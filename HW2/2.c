#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/stat.h>
#define shm_size     32
int main(void)
{
    int shmid;
	key_t key;
	char *shm, *s;
	key = 5679;
	//read only
	if ((shmid = shmget(key, shm_size , S_IRUSR | 0666)) < 0) {
		perror("shmget");
		return 1;
	}
	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
		perror("shmat");
		return 1;
	}
	for (s = shm; *s != NULL; s++)
		putchar(*s);
	putchar('\n');
	*shm = '*';
	return 0;
}
