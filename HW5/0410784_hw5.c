#include<stdlib.h>
#include<stdio.h>
#include<sys/time.h>
typedef struct list{
	int size;
	struct node *first;
	struct node *final;
} List;

typedef struct node{
	struct node *last;
	int data;
	struct node *next;
}Node;
int linearSearch1(int num_arr[], int target, int size){
	for(int i=0;i<size;i++){
		if(num_arr[i] == target)
			return i;
	}
	return -1;
}
int linearSearch2(List *l, int target){
	Node *node = l->first;
	int count = 0;
	while(node != NULL){
		count++;
		if(node->data == target)
			return count;
		node = node->next;
	}
	return -1;
}
int main(){
	/*
		FIFO
	*/
	char filename[30];
	struct timeval start, end;
	printf("Please input the file name: ");
	scanf("%s",filename);

	printf("FIFO---\n");
	printf("size\t\tmiss\t\thit\t\tpage fault ratio\n");
	gettimeofday(&start,0);
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
		fin = fopen(filename,"r");

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
		Node *hash_table[9000];
		long hit=0, miss=0;
		int input_num;
		int count = 0;

		List *list1 = (List*)malloc(sizeof(List));
		list1->size = 0;
		list1->first = NULL;
		list1->final = NULL;		

		fin = fopen(filename,"r");
		for(int i=0 ; i<9000 ; i++)
			hash_table[i] = NULL;
		while(fscanf(fin,"%d",&input_num) != EOF){
			//miss
			if(hash_table[input_num-1000] == NULL){
				Node *ptr = (Node*)malloc(sizeof(Node));
				hash_table[input_num-1000] = ptr;
				ptr->last = NULL;
				ptr->data = input_num;
				ptr->next = list1->first;
				if(list1->first == NULL){ //first node
					list1->first = ptr;
					list1->final = ptr;
				}
				else{
					list1->first->last = ptr;
					list1->first = ptr;
				}
				list1->size++;
				//pop
				if(list1->size > frame){
					Node* final_ptr = list1->final;
					list1->final = list1->final->last;
					list1->final->next = NULL;
					hash_table[final_ptr->data-1000] = NULL;
					free(final_ptr);
					list1->size--;
				}

				miss++;
			}
			//hit
			else{
				Node *ptr = hash_table[input_num-1000];
				if(ptr->last == NULL);
				else if(ptr == list1->final){
					list1->final = ptr->last;
					list1->final->next = NULL;
					ptr->last = NULL;
					ptr->next = list1->first;
					list1->first->last = ptr;
					list1->first = ptr;
				}
				else{
					ptr->last->next = ptr->next;
					ptr->next->last = ptr->last;
					ptr->last = NULL;
					ptr->next = list1->first;
					list1->first->last = ptr;
					list1->first = ptr;
				}

				hit++;
			}
		}

		printf("%d\t\t%ld\t\t%ld\t\t%.9lf\n",frame,miss,hit,miss*1./(hit+miss));
		fclose(fin);
	}
	gettimeofday(&end,0);
	int sec = end.tv_sec - start.tv_sec;
	int usec = end.tv_usec - start.tv_usec;
	printf("Execution time: %.2fsec.\n",1.*sec+usec/1e6);

	/*
		linear search
	*/
	printf("--------Linear Search--------\n");
	gettimeofday(&start,0);
	for(int frame = 128; frame <= 1024; frame*=2){ 
		FILE *fin;
		int *que;
		long hit=0, miss=0;
		int input_num, size=0, index=0;

		que = (int*)malloc(frame*sizeof(int));
		for(int i=0 ; i<frame ; i++)
			que[i] = 0;
		fin = fopen(filename,"r");

		while(fscanf(fin,"%d",&input_num) != EOF){
			//miss
			if(linearSearch1(que, input_num, size) < 0){
				size++;
				//pop
				if(size > frame){
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
		free(que);
		fclose(fin);
	}
	for(int frame=128; frame<=1024; frame*=2){
		FILE *fin;
		Node *hash_table[9000];
		long hit=0, miss=0;
		int input_num;
		int count = 0;

		List *list1 = (List*)malloc(sizeof(List));
		list1->size = 0;
		list1->first = NULL;
		list1->final = NULL;		

		fin = fopen(filename,"r");
		for(int i=0 ; i<9000 ; i++)
			hash_table[i] = NULL;
		while(fscanf(fin,"%d",&input_num) != EOF){
			//miss
			if(linearSearch2(list1, input_num) < 0){
				Node *ptr = (Node*)malloc(sizeof(Node));
				hash_table[input_num-1000] = ptr;
				ptr->last = NULL;
				ptr->data = input_num;
				ptr->next = list1->first;
				if(list1->first == NULL){ //first node
					list1->first = ptr;
					list1->final = ptr;
				}
				else{
					list1->first->last = ptr;
					list1->first = ptr;
				}
				list1->size++;
				//pop
				if(list1->size > frame){
					Node* final_ptr = list1->final;
					list1->final = list1->final->last;
					list1->final->next = NULL;
					hash_table[final_ptr->data-1000] = NULL;
					free(final_ptr);
					list1->size--;
				}

				miss++;
			}
			//hit
			else{
				Node *ptr = hash_table[input_num-1000];
				if(ptr->last == NULL);
				else if(ptr == list1->final){
					list1->final = ptr->last;
					list1->final->next = NULL;
					ptr->last = NULL;
					ptr->next = list1->first;
					list1->first->last = ptr;
					list1->first = ptr;
				}
				else{
					ptr->last->next = ptr->next;
					ptr->next->last = ptr->last;
					ptr->last = NULL;
					ptr->next = list1->first;
					list1->first->last = ptr;
					list1->first = ptr;
				}

				hit++;
			}
		}
	}
	gettimeofday(&end,0);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("Execution time: %.2fsec.\n",1.*sec+usec/1e6);
}
