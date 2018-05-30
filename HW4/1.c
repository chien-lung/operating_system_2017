#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<string.h>
#include<sys/time.h>
void* thread_sort(void* ptr);
int partition(int left, int right);
void bubbleSort(int left, int right);
int read_data(char* filename);
void write_data(int a, int num);
struct Job{
	int left;
	int right;
	int thread_num;
	struct Job* next;
}job_list[15];

int job_exec_index, job_add_index;
struct Job *job_exec, *job_add;
int *data;
int exec_thread_max, exec_thread_num;
sem_t sem_threads, mutex_job, sem_main;

int main(int argc, char* argv[]){
	int num;
	char filename[15];
	sem_init(&sem_threads,0,0);
	sem_init(&mutex_job,0,1);
	sem_init(&sem_main,0,0);
	
	if(argv[1] == NULL)
		strcpy(filename,"input.txt");
	else
		strcpy(filename,argv[1]);
		

	/*
		Create 8 threads in pool
	*/
	pthread_t thread;
	for(int i=0;i<8;i++)
		pthread_create(&thread, NULL, (void*)thread_sort, NULL);
	
	struct timeval start, end;
	for(int i=1;i<=8;i++){
		num = read_data(filename);

		job_exec_index = 0;
		job_add_index = 1;
		//initial first job
		job_list[job_exec_index].left = 0;
		job_list[job_exec_index].right = num - 1;
		job_list[job_exec_index].thread_num = 0;
		job_list[job_exec_index].next = NULL;
		//printf("job[%d]: left:%d, right:%d, th_num:%d\n",job_exec_index,job_list[0].left,job_list[0].right,job_list[0].thread_num);

		job_exec = &job_list[job_exec_index];
		job_add = job_exec;

		exec_thread_num = 1;
		exec_thread_max = i;
		gettimeofday(&start,0);
		sem_post(&sem_threads);
		for(int i=0;i<8;i++)
			sem_wait(&sem_main);
		gettimeofday(&end,0);
		int sec = end.tv_sec - start.tv_sec;
		int usec = end.tv_usec - start.tv_usec;
		printf("%d thread sorting using %.2f sec\n\n", i, sec+usec/1e6);
		write_data(i,num);
	}

	return 0;
}
void* thread_sort(void* ptr){
	int pivot;
	while(1){
		sem_wait(&sem_threads);
		sem_wait(&mutex_job);
		struct Job*	job_current = job_exec;
		
		//no more job need to do
		if(job_current == NULL){
			exec_thread_num--;
			sem_post(&mutex_job);
			continue;
		}
		sem_post(&mutex_job);
		/*
		//let another thread deal with next job
		else{
			job_exec = job_exec->next;
			sem_post(&mutex_job);
		}
		*/

		//partition
		if(job_current->thread_num < 7){
			pivot = partition(job_current->left, job_current->right);
			sem_wait(&mutex_job);
			//new 2 job
			job_list[job_add_index].left = job_current->left;
			job_list[job_add_index].right = pivot - 1;
			job_list[job_add_index].thread_num = job_current->thread_num*2+1;
			job_list[job_add_index].next = NULL;
			printf("job[%d]: left:%d, right:%d, th_num:%d\n",job_add_index,job_list[job_add_index].left,job_list[job_add_index].right,job_list[job_add_index].thread_num);
			job_add->next = &job_list[job_add_index];
			job_add = job_add->next;
			job_add_index = (job_add_index + 1)%15;
			
			job_list[job_add_index].left = pivot + 1;
			job_list[job_add_index].right = job_current->right;
			job_list[job_add_index].thread_num = job_current->thread_num*2+2;
			job_list[job_add_index].next = NULL;
			printf("job[%d]: left:%d, right:%d, th_num:%d\n",job_add_index,job_list[job_add_index].left,job_list[job_add_index].right,job_list[job_add_index].thread_num);
			job_add->next = &job_list[job_add_index];
			job_add = job_add->next;
			job_add_index = (job_add_index + 1)%15;
			//this thread finish job
			exec_thread_num--;

			printf("job_current: left:%d, right:%d, th_num:%d\n",job_current->left, job_current->right, job_current->thread_num);
			printf("job_current->next: left:%d, right:%d, th_num:%d\n",job_current->next->left, job_current->next->right, job_current->next->thread_num);
			
			job_exec = job_exec->next;

			//printf("1\n");
			if(exec_thread_num > exec_thread_max)
				exec_thread_num = exec_thread_max;
			else{
				//printf("2\n");
				for(int i=0;i<exec_thread_max-exec_thread_num;i++){
					//printf("3\n");
					sem_post(&sem_threads);
					exec_thread_num++;
				}
			}
			//printf("4\n");
			sem_post(&mutex_job);

		}
		
		//bubble sort
		if(job_current->thread_num > 6){
			bubbleSort(job_current->left, job_current->right);
			sem_wait(&mutex_job);
			exec_thread_num--;
			if(exec_thread_num > exec_thread_max)
				exec_thread_num = exec_thread_max;
			else{
				for(int i=0;i<exec_thread_max-exec_thread_num;i++){
					sem_post(&sem_threads);
					exec_thread_num++;
				}
			}
			sem_post(&mutex_job);
			sem_post(&sem_main);
		}
			
	}
}

int partition(int left, int right){
	if(left >= right)
		return left;
	int i=left, j=right;
	int tmp;
	int pivot = data[i];
	while(i<=j){
		while(data[i] <= pivot)
			i++;
		while(data[j] > pivot)
			j--;
		if(i<j){
			tmp = data[i];
			data[i] = data[j];
			data[j] = tmp;
			i++;
			j--;
		}
		else if(i==j)
			j--;
	}
	tmp = data[left];
	data[left] = data[j];
	data[j] = tmp;
	return j;
}
void bubbleSort(int left, int right){
	int N = right-left+1;
	for(int i=0 ; i<N-1 ; i++)
		for(int j=left ; j<right-i ; j++)
			if(data[j] > data[j+1]){
				int tmp = data[j];
				data[j] = data[j+1];
				data[j+1] = tmp;
			}
}

int read_data(char* filename){
	int num;
	FILE *fin;
	fin = fopen(filename,"r");
	fscanf(fin,"%d",&num);
	data = (int*)malloc(num*sizeof(int));
	for(int i=0;i<num;i++){
		fscanf(fin,"%d ",&data[i]);
	}
	fclose(fin);	

	return num;
}
void write_data(int a, int num){
	FILE* fout;
	char filename[15];
	sprintf(filename,"output_%d.txt",a);
	fout = fopen(filename,"w");
	for(int i=0;i<num;i++)
		fprintf(fout,"%d ",data[i]);
	fclose(fout);
}
