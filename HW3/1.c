#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#include<time.h>
#define THREAD_NUM 15
void quickSort(int arr[], int left, int right);
void bubbleSort(int arr[], int N);
int left[THREAD_NUM];
int right[THREAD_NUM];
sem_t *sem = (sem_t*)malloc(THREAD_NUM*sizeof(sem_t));

int main(int argc, char* argv[]){
	FILE *fin, *fout1, *fout2;
	int *nums;
	int  N; 
	pthread_t thread[THREAD_NUM];
	int sec,usec;
	struct timeval start, end;

	srand(time(NULL));
	fin = fopen(argv[1],"r");
	fout1 = fopen("output1.txt","w");
	fout2 = fopen("output2.txt","w");

	fscanf(fin,"%d", &N);
	nums = (int*)malloc(N*sizeof(int));
	for(int i=0;i<N;i++){
		fscanf(fin,"%d ",&nums[i]);
	}
	fclose(fin);

	for(int i=0;i<THREAD_NUM;i++){
		pthread_create(&thread[i],NULL,quickSort,NULL);
	}
	
	gettimeofday(&start,0);
	int left=0, right=N-1;
	quickSort(nums,left,right);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout1,"%d ",nums[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("quickSort time(%d datas): %fsec.\n",N,sec+usec/1000000.0);
	
	
	gettimeofday(&start,0);
	bubbleSort(nums,N);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout2,"%d ",nums[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("bubbleSort time(%d datas): %fsec.\n",N,sec+usec/1000000.0);
	
	
	fclose(fout1);
	fclose(fout2);

}
void quickSort(int arr[], int left, int right){
	int i=left, j=right;
	int tmp;
	int pivot = arr[left];
	
	while(i<=j){
		while(arr[i] < pivot)
			i++;
		while(arr[j] > pivot)
			j--;
		if(i<=j){
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
			i++;
			j--;
		}
	}
	
	//if(left < j)
	//	quickSort(arr, left, j);
	//if(i < right)
	//	quickSort(arr, i, right);
}
void bubbleSort(int arr[], int N){
	for(int i=0;i<N-1;i++)
		for(int j=0;j<N-i-1;j++)
			if(arr[j]>arr[j+1]){
				int tmp=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=tmp;
			}
}
