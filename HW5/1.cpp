#include<stdlib.h>
#include<stdio.h>
#include<list>
using namespace std;
int main(){
	/*
		FIFO
	*/
	/*
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
	*/
	/*
		LRU
	*/
	printf("LRU---\n");
	printf("size\t\tmiss\t\thit\t\tpage fault ratio\n");
	for(int frame=128; frame<=1024; frame*=2){
		FILE *fin;
		int *hash_table[9000];
		long hit=0, miss=0;
		int input_num;
		list<int> lru;

		fin = fopen("trace.txt","r");
		for(int i=0 ; i<9000 ; i++)
			hash_table[i] = NULL;

		while(fscanf(fin,"%d",&input_num) != EOF){
		}
		printf("%d\t\t%ld\t\t%ld\t\t%.9lf\n",frame,miss,hit,miss*1./(hit+miss));
		fclose(fin);
	}
}
