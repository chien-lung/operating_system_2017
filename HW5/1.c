#include<stdlib.h>
#include<stdio.h>
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

void printNode1(List* l){
	Node *node=l->first;
	while(node != NULL){
		printf("%d-",node->data);
		node = node->next;
	}
	printf("\n");
}
void printNode2(List* l){
	Node *node=l->final;
	while(node != NULL){
		printf("%d-",node->data);
		node = node->last;
	}
	printf("\n");
}
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
		Node *hash_table[9000];
		long hit=0, miss=0;
		int input_num;
		int count = 0;

		List *list1 = (List*)malloc(sizeof(List));
		list1->size = 0;
		list1->first = NULL;
		list1->final = NULL;		

		fin = fopen("trace.txt","r");
		for(int i=0 ; i<9000 ; i++)
			hash_table[i] = NULL;
		while(fscanf(fin,"%d",&input_num) != EOF){
			//if(count++ == 50) exit(0);
			//printf("%d  ",count);
			//printf("num: %d\t",input_num);
			//miss
			if(hash_table[input_num-1000] == NULL){
				//printf("miss\t");
				Node *ptr = (Node*)malloc(sizeof(Node));
				hash_table[input_num-1000] = ptr;
				ptr->last = NULL;
				ptr->data = input_num;
				ptr->next = list1->first;
				if(list1->first == NULL){ //first node
					//printf("2-1\t");
					list1->first = ptr;
					list1->final = ptr;
				}
				else{
					//printf("2-2\t");
					list1->first->last = ptr;
					list1->first = ptr;
				}
				list1->size++;
				//printf("size: %d\n",list1->size);
				//pop
				if(list1->size > frame){
					//printf("2-4\n");
					Node* final_ptr = list1->final;
					//printf("2-4-1  ");
					list1->final = list1->final->last;
					//printf("2-4-2  ");
					list1->final->next = NULL;
					//printf("2-4-3  ");
					hash_table[final_ptr->data-1000] = NULL;
					free(final_ptr);
					list1->size--;
				}

				miss++;
				//printf("\n");
			}
			//hit
			else{
				Node *ptr = hash_table[input_num-1000];
				//printf("hit\t");
				if(ptr->last == NULL);// printf("3-1\n");
				else if(ptr == list1->final){
					//printf("3-2\n");
					list1->final = ptr->last;
					list1->final->next = NULL;
					ptr->last = NULL;
					ptr->next = list1->first;
					list1->first->last = ptr;
					list1->first = ptr;
				}
				else{
					//printf("3-3\n");
					ptr->last->next = ptr->next;
					ptr->next->last = ptr->last;
					ptr->last = NULL;
					ptr->next = list1->first;
					list1->first->last = ptr;
					list1->first = ptr;
				}

				hit++;
			}
			//printNode1(list1);
			//printNode2(list1);
		}

		printf("%d\t\t%ld\t\t%ld\t\t%.9lf\n",frame,miss,hit,miss*1./(hit+miss));
		fclose(fin);
	}
}
