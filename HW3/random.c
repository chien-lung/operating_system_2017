#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#include<time.h>
#define N 100
int main(){
	FILE *fout;
	int sec,usec;
	struct timeval start, end;

	srand(time(NULL));
	fout = fopen("input2.txt","w");
	
	fprintf(fout,"%d \n",N);
	for(int i=0;i<N;i++){
		fprintf(fout,"%d ",rand()%1000+1);
	}

	fclose(fout);

}
