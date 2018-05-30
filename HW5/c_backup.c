#include<stdlib.h>
#include<stdio.h>
#include<time.h>
//void quickSort(int *num_arr, int left, int right);
//int binarySearch(int *num_arr, int num, int target);
void count(int *arr){
	int a=0;
	for(int i=0;i<9000;i++)
		if(arr[i]==1) a++;
	printf("%d\n",a);
}
int main(){
	/*
		FIFO
	*/	
	printf("FIFO---\n");
	printf("size\t\tmiss\t\thit\t\tpage fault ratio\n");
	for(int frame = 128; frame <= 1024; frame*=2){ 
		FILE *fin;
		int hash_table[9000];
		int *que;
		long hit=0, miss=0;
		int input_num, size=0, index=0;

		que = (int*)malloc(frame*sizeof(int));
		for(int i=0 ; i<9000 ; i++)
			hash_table[i] = 0;
		for(int i=0 ; i<frame ; i++)
			que[i] = 0;
		fin = fopen("trace.txt","r");

		while(fscanf(fin,"%d",&input_num) != EOF){
			//miss
			if(hash_table[input_num-1000] == 0){
				hash_table[input_num-1000] = 1;
				size++;
				//pop
				if(size > frame){
					hash_table[ que[index]-1000 ] = 0;
					size--;
				}
				//push
				que[index] = input_num;
				index = (index + 1)%frame;
				miss++;
			}
			//hit
			else 
				hit++;
		}
		printf("%d\t\t%ld\t\t%ld\t\t%.9lf\n",frame,miss,hit,miss*1./(hit+miss));
		free(que);
		fclose(fin);
	}

	/*
		LRU
	*/
	printf("LRU---\n");
	printf("size\t\tmiss\t\thit\t\tpage fault ratio\n");
	for(int frame=128; frame<=1024; frame*=2){
		FILE *fin;
		int *hash_table[9000];
		long hit=0, miss=0;
		int input_num, size=0, index=0;

		for(int i=0 ; i<9000 ; i++)
			hash_table[i] = NULL;

		printf("%d\t\t%ld\t\t%ld\t\t%.9lf\n",frame,miss,hit,miss*1./(hit+miss));
		fclose(fin);
	}
}
/*
int binarySearch(int *num_arr, int num, int target){
	int left=0, right=num-1,mid;
	while(left <= right){
		mid = (left+right)/2;
		if(num_arr[mid] == target) 
			return mid;
		else if(num_arr[mid] > target)
			right = mid-1;
		else
			left = mid+1;
	}
	return -1;
}
void quickSort(int *num_arr, int left, int right){
	int i=left, j = right;
	int pivot = num_arr[i];
	if(left >= right) return;
	while(i<=j){
		while(num_arr[i] < pivot)
			i++;
		while(num_arr[j] > pivot)
			j--;
		if(i<=j){
			int tmp = num_arr[i];
			num_arr[i] = num_arr[j];
			num_arr[j] = tmp;
			i++;
			j--;
		}
	}
	quickSort(num_arr, left, j);
	quickSort(num_arr, i , right);
}*/
