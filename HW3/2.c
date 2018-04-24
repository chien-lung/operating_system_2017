#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#include<time.h>
#define N 100000
void quickSort(int arr[], int left, int right);
void bubbleSort(int arr[]);
int main(){
	FILE *fin, *fout1, *fout2;
	int nums1[N],nums2[N];
	int left=0, right=N-1;
	int sec,usec;
	struct timeval start, end;

	srand(time(NULL));
	//fin = fopen("input.txt","r");
	fout1 = fopen("output1.txt","w");
	fout2 = fopen("output2.txt","w");

	for(int i=0;i<N;i++){
		nums1[i] = nums2[i] = rand()%1000000+1;
	}
	gettimeofday(&start,0);
	quickSort(nums1,left,right);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout1,"%d ",nums1[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("quickSort time(%d datas): %fsec.\n",N,sec+usec/1000000.0);
	

	gettimeofday(&start,0);
	bubbleSort(nums2);
	gettimeofday(&end,0);
	for(int i=0;i<N;i++)
		fprintf(fout2,"%d ",nums2[i]);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("bubbleSort time(%d datas): %fsec.\n",N,sec+usec/1000000.0);

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

	if(left < j)
		quickSort(arr, left, j);
	if(i < right)
		quickSort(arr, i, right);
}
void bubbleSort(int arr[]){
	for(int i=0;i<N-1;i++)
		for(int j=0;j<N-i-1;j++)
			if(arr[j]>arr[j+1]){
				int tmp=arr[j];
				arr[j]=arr[j+1];
				arr[j+1]=tmp;
			}
}
