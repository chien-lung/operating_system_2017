#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/stat.h>
#define shm_size     32
int main(void)
{
    char c;
	int shm_id;
	char *shm_addr, *s;
	key_t key;

	key = 5679;
	if ((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) < 0) {
		perror("shmget");
		return 0;
	}
	//printf("1: shm_id:%d\n",shm_id);
	if ((shm_addr = shmat(shm_id, NULL, 0)) == (char *) -1) {
		perror("shmat");
		return 1;
	}
	s = shm_addr;
	for (c = 'a'; c <= 'z'; c++)
		*s++ = c;
	*s = NULL;
	while (*shm_addr != '*'){
		sleep(1);
	}
	shmdt(shm_addr);
	shmctl(shm_id , IPC_RMID , NULL);
	return 0;
}
