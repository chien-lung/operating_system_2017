#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#define THREAD_NUMS 15
#define USING_QUICKSORT 1
#define USING_BUBBLESORT 2
void *quickSort(void* thread_num_ptr/*int arr[], int left, int right*/);

int left[THREAD_NUMS];
int right[THREAD_NUMS];
int *nums;
sem_t sem[THREAD_NUMS];
sem_t sem_main;

int main(int argc, char* argv[]){
	FILE *fin, *fout1, *fout2;
	int  N; 
	pthread_t thread[THREAD_NUMS];
	int sec,usec;
	struct timeval start, end;
	int thread_num, *thread_num_ptr;

	fin = fopen(argv[1],"r");
	fout1 = fopen("output1.txt","w");
	fout2 = fopen("output2.txt","w");

	/*
	read array data from file
	*/
	fscanf(fin,"%d", &N);
	nums = (int*)malloc(N*sizeof(int));
	for(int i=0;i<N;i++){
		fscanf(fin,"%d ",&nums[i]);
	}
	fclose(fin);

	/*
	create 15 threads, 15 semaphores, and main semaphore
	*/
	sem_init(&sem_main,0,0);
	for(thread_num=0 ; thread_num<THREAD_NUMS ; thread_num++){
		left[thread_num] = right[thread_num] = 0;
		thread_num_ptr = &thread_num;
		sem_init(&sem[thread_num],0,0);
		pthread_create(&thread[thread_num], NULL, quickSort, (void*)thread_num_ptr);
	}

	left[0]=0;
	right[0]=N-1;
	
	gettimeofday(&start,0);
	//signal first thread to start
	sem_post(&sem[0]);
	//wait for all thread finish
	sem_wait(&sem_main);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout1,"%d ",nums[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("quickSort time(%d datas): %fsec.\n",N,sec+usec/1000000.0);

	/*
	gettimeofday(&start,0);
	int left=0, right=N-1;
	quickSort(nums,left,right);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout1,"%d ",nums[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("quickSort time(%d datas): %fsec.\n",N,sec+usec/1000000.0);
	*/
	
	fclose(fout1);
	fclose(fout2);

}
void *quickSort(void* thread_num_ptr/*int arr[], int left, int right*/){
	
	int thread_num = *(int*)thread_num_ptr;
	int sort;
	if(thread_num < 7) //0~6
		sort = USING_QUICKSORT;
	else //7~14
		sort = USING_BUBBLESORT;
	printf("thread_num: %d\n",thread_num);
	
	/*
	wait for signal
	*/
	printf("thread %d is waiting\n",thread_num);
	sem_wait(&sem[thread_num]);
	printf("thread %d starts\n",thread_num);
	
	if(sort == USING_QUICKSORT){
		int i=left[thread_num], j=right[thread_num];
		int tmp;
		int pivot = nums[i];
	
		while(i<=j){
			while(nums[i] < pivot)
				i++;
			while(nums[j] > pivot)
				j--;
			if(i<=j){
				tmp = nums[i];
				nums[i] = nums[j];
				nums[j] = tmp;
				i++;
				j--;
			}
		}
	
		//signal left to start
		left[2*thread_num+1] = left[thread_num];
		right[2*thread_num+1] = j;
		sem_post(&sem[2*thread_num+1]);
		printf("Activate thread %d\n",2*thread_num+1);
		
		//signal right to start
		left[2*thread_num+2] = i;
		right[2*thread_num+2] = right[thread_num];
		sem_post(&sem[2*thread_num+2]);
		printf("Activate thread %d\n",2*thread_num+2);

		if(thread_num == 0){
			//wait 7~14 thread finish sort
			for(int k=7; k<THREAD_NUMS ;k++)
				sem_wait(&sem[k]);
			//signal main to continue and output
			sem_post(&sem_main);
			
			printf("thread %d exit\n",thread_num);
			pthread_exit(NULL);
		}
		else{ //thread_num 1~6
			printf("thread %d exit\n",thread_num);
			pthread_exit(NULL);
		}
		
	}

	else{ //sort == USING_BUBBLESORT
		int N = right[thread_num]-left[thread_num]+1;

		for(int i=left[thread_num] ; i<N-1 ; i++)
			for(int j=0 ; j<N-i-1 ; j++)
				if(nums[j] > nums[j+1]){
					int tmp = nums[j];
					nums[j] = nums[j+1];
					nums[j+1] = tmp;
				}
		printf("thread %d exit\n",thread_num);
		pthread_exit(NULL);

	}

}
