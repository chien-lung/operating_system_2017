#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<sys/time.h>
#define THREAD_NUMS 15
#define USING_QUICKSORT 1
#define USING_BUBBLESORT 2
void *partitionAndSort(void* thread_num_ptr);
void *quickAndBubbleSort(void* ptr);
void bubbleSort(int t_num);
void partition(int t_num); 


int left[THREAD_NUMS];
int right[THREAD_NUMS];
int *nums, *nums2;
sem_t sem[THREAD_NUMS];
sem_t sem_main, sem_st;

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
	nums2 = (int*)malloc(N*sizeof(int));
	for(int i=0;i<N;i++){
		fscanf(fin,"%d ",&nums[i]);
		nums2[i] = nums[i];
	}
	fclose(fin);

	/*
		Multi-thread (15 threads)
	*/

	
	//create 15 threads, 15 semaphores, and main semaphore
	sem_init(&sem_main,0,0);
	for(thread_num=0 ; thread_num<THREAD_NUMS ; thread_num++){
		left[thread_num] = right[thread_num] = 0;
		thread_num_ptr = &thread_num;
		sem_init(&sem[thread_num],0,0);
		pthread_create(&thread[thread_num], NULL, partitionAndSort, (void*)thread_num_ptr);
		usleep(150);
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
	printf("Multi-Thread sorting time(%d datas): %.2fsec.\n",N,sec+usec/1000000.0);	
	
	fclose(fout1);
	free(nums);
	for(int i=0;i<THREAD_NUMS;i++)
		sem_destroy(&sem[i]);

	/*
		Single-thread
	*/
	pthread_t singleThread;
	sem_init(&sem_st,0,0);
	pthread_create(&singleThread, NULL, quickAndBubbleSort, NULL);
	gettimeofday(&start,0);
	sem_post(&sem_st);
	sem_wait(&sem_main);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout2,"%d ",nums2[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("Single-Thread sorting time(%d datas): %.2fsec.\n",N,sec+usec/1000000.0);	
		
	fclose(fout2);

}
void *quickAndBubbleSort(void* ptr){
	sem_wait(&sem_st);
	partition(0);
	for(int i=7;i<THREAD_NUMS;i++)
		bubbleSort(i);
	sem_post(&sem_main);
	pthread_exit(NULL);	
}
void partition(int t_num){
	if(t_num < 7){
		int i=left[t_num], j=right[t_num];
		int tmp;
		int pivot = nums2[i];
		while(i<=j){
			while(nums2[i] <= pivot)
				i++;
			while(nums2[j] > pivot)
				j--;
			if(i<j){
				tmp = nums2[i];
				nums2[i] = nums2[j];
				nums2[j] = tmp;
				i++;
				j--;
			}
			else if(i==j)
				j--;
		}
		tmp = nums2[left[t_num]];
		nums2[left[t_num]] = nums2[j];
		nums2[j] = tmp;

		printf("pivot:%d\n",pivot);
		printf("num2[i]:%d, nums2[j]:%d\n",nums2[i],nums2[j]);
		left[2*t_num+1] = left[t_num];
		right[2*t_num+1] = j;
			
		left[2*t_num+2] = i;
		right[2*t_num+2] = right[t_num];

		partition(2*t_num+1);
		partition(2*t_num+2);
	}
}
void bubbleSort(int t_num){
	int N = right[t_num]-left[t_num]+1;

	for(int i=0 ; i<N-1 ; i++)
		for(int j=left[t_num] ; j<right[t_num]-i ; j++)
			if(nums2[j] > nums2[j+1]){
				int tmp = nums2[j];
				nums2[j] = nums2[j+1];
				nums2[j+1] = tmp;
			}
}
void *partitionAndSort(void* thread_num_ptr){
	
	int thread_num = *(int*)thread_num_ptr;
	int sort;
	if(thread_num < 7) //0~6
		sort = USING_QUICKSORT;
	else //7~14
		sort = USING_BUBBLESORT;
	
	
	//wait for signal
	sem_wait(&sem[thread_num]);
	
	if(sort == USING_QUICKSORT){
		int i=left[thread_num], j=right[thread_num];
		int tmp;
		int pivot = nums[left[thread_num]];
		
		while(i<=j){
			while(nums[i] <= pivot)
				i++;
			while(nums[j] > pivot)
				j--;
			if(i<j){
				tmp = nums[i];
				nums[i] = nums[j];
				nums[j] = tmp;
				i++;
				j--;
			}
			else if(i==j){
				//i++;
				j--;
			}
		}
		
		//signal left to start
		left[2*thread_num+1] = left[thread_num];
		right[2*thread_num+1] = j;
		sem_post(&sem[2*thread_num+1]);
		
		//signal right to start
		left[2*thread_num+2] = i;
		right[2*thread_num+2] = right[thread_num];
		sem_post(&sem[2*thread_num+2]);

		if(thread_num == 0){
			//wait 7~14 thread finish sort
			for(int k=7; k<THREAD_NUMS ;k++)
				sem_wait(&sem[k]);
			//signal main to continue and output
			sem_post(&sem_main);
			
			pthread_exit(NULL);
		}
		else{ //thread_num 1~6
			pthread_exit(NULL);
		}
		
	}

	else{ //sort == USING_BUBBLESORT
		int N = right[thread_num]-left[thread_num]+1;

		for(int i=0 ; i<N-1 ; i++)
			for(int j=left[thread_num] ; j<right[thread_num]-i ; j++)
				if(nums[j] > nums[j+1]){
					int tmp = nums[j];
					nums[j] = nums[j+1];
					nums[j+1] = tmp;
				}
		sem_post(&sem[thread_num]);
		pthread_exit(NULL);

	}

}
