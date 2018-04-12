#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
void getAndAttachShm(int *shm_id, unsigned int **shm_addr, int matrix_dim);
void detachAndReleaseShm(int shm_id, unsigned int *shm_addr);
unsigned int processing(int matrix_dim, int p_nums);
unsigned int *A,*B,*C;
int main()
{
	unsigned int sum;
	int matrix_dim, shm_id;
	unsigned int* shm_addr;
	key_t key;//key = IPC_PRIVATE for parent-child process
	struct timeval start, end;
	int sec, usec;
	
	printf("Input matrix dimension: ");
	scanf("%d",&matrix_dim);
	
	for(int i=1;i<=16;i++){
		gettimeofday(&start, 0);
		sum = processing(matrix_dim, i);	
		gettimeofday(&end, 0);
		sec = end.tv_sec - start.tv_sec;
		usec = end.tv_usec - start.tv_usec;
		printf("Multiplying matrices using %d process(es)\n",i);
		printf("Elapsed time: %f sec. Checksum : %u\n",sec+usec/1000000.0, sum);
	}
	
	return 0;

}

void getAndAttachShm(int *shm_id, unsigned int **shm_addr, int matrix_dim){
	int matrix_size = matrix_dim*matrix_dim;
	int shm_size = 3*matrix_size*sizeof(unsigned int);

	if(( *shm_id = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666)) < 0 ){
		perror("shmget error");
		exit(-1);
	}
	if(( *shm_addr = shmat(*shm_id, NULL, 0)) == (unsigned int*)-1){
		perror("shmat error");
		exit(-1);
	}
	A = *shm_addr;
	B = *shm_addr+matrix_size;
	C = *shm_addr+2*matrix_size;

	for(int i=0;i<matrix_dim;i++)
		for(int j=0;j<matrix_dim;j++){
			A[i*matrix_dim + j] = i*matrix_dim + j;
			B[i*matrix_dim + j] = i*matrix_dim + j;
		}
}
void detachAndReleaseShm(int shm_id, unsigned int *shm_addr){
	
	if(shmdt(shm_addr) < 0){
		perror("shmdt error");
		exit(-1);
	}
	if(shmctl(shm_id, IPC_RMID, NULL) < 0){
		perror("shmctl error");
		exit(-1);
		
	}
}
unsigned int processing(int matrix_dim, int p_nums){
	int shm_id;
	unsigned int *shm_addr;
	unsigned int sum = 0;
	pid_t *pid = (pid_t*)malloc(p_nums*sizeof(pid_t));
	
	getAndAttachShm(&shm_id, &shm_addr, matrix_dim);
	
	for(int i=0;i<p_nums;i++){
		//child
		if( (pid[i] = fork()) == 0){
			for(int j=matrix_dim*i/p_nums ; j<matrix_dim*(i+1)/p_nums ; j++)
				for(int k=0 ; k<matrix_dim ; k++)
					for(int m=0 ; m<matrix_dim ; m++)
						C[j*matrix_dim+k] += A[j*matrix_dim+m]*B[m*matrix_dim+k];
			shmdt(shm_addr);
			exit(0);
		}

	}
	//parent
	for(int i=0;i<p_nums;i++)
		wait(NULL);
	
	for(int i=0 ; i<matrix_dim ; i++)
		for(int j=0 ; j<matrix_dim ; j++)
			sum += C[i*matrix_dim+j];

	detachAndReleaseShm(shm_id, shm_addr);
	
	return sum;
}
