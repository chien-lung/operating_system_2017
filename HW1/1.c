#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(){
	pid_t pid;
	//fork another process
	pid = fork();
	if(pid < 0){ //error occurred
		fprintf(stderr,"Fork Failed");
		exit(-1);
	}
	else if(pid == 0){ //child process
		char* p[] = {"ls",NULL};
		execvp("ls",p);
		//execlp("/bin/ls","ls",NULL);
		exit(0);
	}
	else{ //parent process
		wait(NULL);
		exit(0);
	}
}
