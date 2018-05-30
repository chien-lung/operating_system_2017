#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#include<stdbool.h>
int data[1000001];
pthread_t thread[8];
sem_t mutex_s;
sem_t mutex_t;
sem_t mutex_bdone;
int job_global;
int m;
int job_add;
int thread_max;
struct jobs
{
	int start;
	int end;
	int flagqb;
}job_list[100];
void bubble_sort(int start, int len) {
	//printf("busort left: %d  right: %d\n",start,start + len);
	int i, j, temp;
	for (i = 0; i < len - 1; i++)
		for (j = 0; j < len - 1 - i; j++)
			if (data[start + j] > data[start + j + 1]) {
				temp = data[start + j];
				data[start + j] = data[start + j + 1];
				data[start + j + 1] = temp;
			}
}
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
int quicksort(int *data, int left, int right)
{
	//printf("qksort left: %d  right: %d\n",left,right);
    int pivot, i, j;
    if (left >= right) { return left; }
    pivot = data[left];
    i = left + 1;
    j = right;
    while (1)
    {
        while (i <= right)
        {
            if (data[i] > pivot)
                break;
            i = i + 1;
        }
        while (j > left)
        {
            if (data[j] < pivot)
                break;
            j = j - 1;
        }
        if (i > j)  break; 
        swap(&data[i], &data[j]);
    }
    swap(&data[left], &data[j]);
    return j;
}
void read_file(char* filename,int* num)
{
	FILE *fp;
	fp = fopen(filename, "rt");
	fscanf(fp,"%d", num);
	for(int i=0; i < *num; i++)
		fscanf(fp, "%d", &data[i]);
	fclose(fp);
}
void write_file(char* filename,int num)
{
	FILE *fp;
	fp = fopen (filename, "wt");
	for(int i=0;i < num; i++)
	{	
		fprintf(fp,"%d ",data[i]);
	}
	fclose(fp);
}
void* thread_sort(void * ptr)
{
	int pivot;
	while(true)
	{
		sem_wait(&mutex_s);
		sem_wait(&mutex_t);
		int job_current = job_global;
		if(job_current != job_add)
		{	
			job_global++;
			sem_post(&mutex_t);
		}
		else
		{
			m--;
			sem_post(&mutex_t);
			continue;
		}
		if(job_list[job_current].flagqb <= 2)
		{
			pivot = quicksort(data, job_list[job_current].start, job_list[job_current].end);
			sem_wait(&mutex_t);
			job_list[job_add].start = job_list[job_current].start;
			job_list[job_add].end = pivot - 1;
			job_list[job_add].flagqb = job_list[job_current].flagqb + 1;
			job_add++;
			job_list[job_add].start = pivot + 1;
			job_list[job_add].end = job_list[job_current].end;
			job_list[job_add].flagqb = job_list[job_current].flagqb + 1;
			job_add++;
			m--;			
			if(m >= thread_max)
				m = thread_max;
			else
				for(int i = 0;i < thread_max - m; i++)
				{
					sem_post(&mutex_s);
					m++;
				}
			sem_post(&mutex_t);
		}
		else
		{
			bubble_sort(job_list[job_current].start, job_list[job_current].end - job_list[job_current].start + 1);
			sem_wait(&mutex_t);
			m--;
			if(m >= thread_max)
				m = thread_max;
			else
				for(int i = 0;i < thread_max - m; i++)
				{
					sem_post(&mutex_s);
					m++;
				}
			sem_post(&mutex_t);
			sem_post(&mutex_bdone);
		}
	}
}
void wake_thread(int thread_num)
{
	pthread_t threadID;
	for(int i=0;i < thread_num;i++)
		pthread_create (&threadID, NULL, (void *) thread_sort, NULL);
}
int main()
{
	int num;
	struct timeval start,end;
	char filename[100];
	char outfilename[100];
	sem_init(&mutex_s, 0, 0);
	sem_init(&mutex_t, 0, 1);		
	sem_init(&mutex_bdone, 0, 0);
	wake_thread(8);
	scanf("%s", filename);
	for(int i=1;i <= 8 ;i++)
	{
		job_global = 1;
		job_add = 2;
		read_file(filename,&num);
		job_list[1].start = 0;
		job_list[1].end = num - 1;
		job_list[1].flagqb = 0;
		gettimeofday(&start,0);
		m = 1;
		thread_max = i;
		sem_post(&mutex_s);
		for(int j=0; j<8;j++)
			sem_wait(&mutex_bdone);
		gettimeofday(&end, 0);
		int sec =end.tv_sec -start.tv_sec;
		int usec = end.tv_usec - start.tv_usec;
		printf("%d thread sorting using %f sec\n",i,sec + (usec / 1e6));
		sprintf(outfilename,"output_%d.txt",i);
		write_file(outfilename, num);
	}
	return 0;
}

