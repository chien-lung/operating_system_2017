#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
unsigned int *A,*B,*C;
int main()
{
	unsigned int sum = 0;
	unsigned int matrix_dim, matrix_size, shm_size;
	int shm_id;
	unsigned int* shm_addr;
	key_t key;//key = IPC_PRIVATE for parent-child process
	pid_t pid;
	struct timeval start, end;
	int sec, usec;

	printf("Input matrix dimension: ");
	scanf("%d",&matrix_dim);
	matrix_size = matrix_dim*matrix_dim;
	shm_size = 3*matrix_size*sizeof(unsigned int);
	//printf("matrix_size: %d\nshm_size: %d\n",matrix_size,shm_size);
	
	/*
	ONE PROCESS
	*/
	
	gettimeofday(&start, 0);

	if((shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666)) < 0 ){
		perror("shmget error");
		exit(-1);
	}
	//printf("shm_id: %d\n",shm_id);
	if((shm_addr = shmat(shm_id, NULL, 0)) == (unsigned int*)-1){
		perror("shmat error");
		exit(-1);
	}
	A = shm_addr;
	B = shm_addr+matrix_size;
	C = shm_addr+2*matrix_size;

	for(int i=0;i<matrix_dim;i++)
		for(int j=0;j<matrix_dim;j++){
			//printf("%d\n",i*matrix_dim+j);
			A[i*matrix_dim + j] = i*matrix_dim + j;
			B[i*matrix_dim + j] = i*matrix_dim + j;
		}
	
	for(int i=0 ; i<matrix_dim ; i++)
		for(int j=0 ; j<matrix_dim ; j++){
				C[i*matrix_dim + j] = 0;
			for(int k=0 ; k<matrix_dim ; k++)
				C[i*matrix_dim+j] += A[i*matrix_dim+k]*B[k*matrix_dim+j];
	}
	for(int i=0;i<matrix_dim;i++)
		for(int j=0;j<matrix_dim;j++)
			sum += C[i*matrix_dim+j];
	
	shmdt(shm_addr);
	shmctl(shm_id, IPC_RMID, NULL);
	
	gettimeofday(&end, 0);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("1: %f sec\n",(sec+usec/1000000.0));
	printf("1 sum : %u\n",sum);
	
	
	/*
	TWO PROCESS
	*/
	
	sum = 0;
	gettimeofday(&start, 0);
	
	if((shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666)) < 0 ){
		perror("shmget error");
		exit(-1);
	}
	//printf("shm_id: %d\n",shm_id);
	if((shm_addr = shmat(shm_id, NULL, 0)) == (unsigned int*)-1){
		perror("shmat error");
		exit(-1);
	}
	A = shm_addr;
	B = shm_addr+matrix_size;
	C = shm_addr+2*matrix_size;

	for(int i=0;i<matrix_dim;i++)
		for(int j=0;j<matrix_dim;j++){
			//printf("%d\n",i*matrix_dim+j);
			A[i*matrix_dim + j] = i*matrix_dim + j;
			B[i*matrix_dim + j] = i*matrix_dim + j;
		}
	if((pid = fork()) < 0){
		fprintf(stderr,"fork fail");
		exit(-1);
	}
	//child process
	else if(pid == 0){
		
		for(int i=0 ; i<matrix_dim/2 ; i++)
			for(int j=0 ; j<matrix_dim ; j++)
				for(int k=0 ; k<matrix_dim ; k++)
					C[i*matrix_dim+j] += A[i*matrix_dim+k]*B[k*matrix_dim+j];
		shmdt(shm_addr);
		exit(0);
	}
	//parent process
	else{
		for(int i=matrix_dim/2 ; i<matrix_dim ; i++)
			for(int j=0 ; j<matrix_dim ; j++)
				for(int k=0 ; k<matrix_dim ; k++)
					C[i*matrix_dim+j] += A[i*matrix_dim+k]*B[k*matrix_dim+j];
		wait(NULL);

		for(int i=0 ; i<matrix_dim ; i++)
			for(int j=0 ; j<matrix_dim ; j++)
				sum += C[i*matrix_dim+j];
		shmdt(shm_addr);
		shmctl(shm_id, IPC_RMID, NULL);
		
		gettimeofday(&end, 0);
		sec = end.tv_sec - start.tv_sec;
		usec = end.tv_usec - start.tv_usec;
		printf("2: %f sec\n",(sec+usec/1000000.0));
		printf("2 sum : %u\n",sum);
	}
	return 0;

}
