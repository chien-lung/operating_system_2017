#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
void shellLoop();
char* readline();
char** cutline();
int execute(char**);

int pos;

int main(){
	shellLoop();
	return 0;
}

void shellLoop(){
	char* line;
	char** tokens;
	int status;
	do{
		printf("> ");
		line = readline();
		tokens = cutline(line);
		status = execute(tokens);

		free(line);
		free(tokens);
	}while(status);
}
int execute(char** tokens){
	
	int np = 0, io = 0, pp = 0;
	int oldfd;
	int pfd[2];
	int status;
	char* ioFilename;
	char** secondCommand;
	
	if(tokens[0] == NULL){
		return 1;
	}
	else if(strcmp(tokens[0],"exit") == 0){
		return 0;
	}
	else if(pos >= 1 && strcmp(tokens[pos-1],"&") == 0){
		np = 1;
		tokens[pos-1] = '\0';
	}
	else if(pos >= 2 &&  strcmp(tokens[pos-2],">") == 0 ){
		io = 1;
		ioFilename = tokens[pos-1];
		tokens[pos-2] = '\0';
	}
	else if(pos >= 2 && strcmp(tokens[pos-2],"|")==0 ){
		pp = 1;
		pipe(pfd);
		tokens[pos-2] = '\0';
		secondCommand = tokens+(pos-1);
	}

	pid_t pid1, pid2;
	pid1 = fork();
	
	if(pid1 < 0){
		fprintf(stderr,"Fork failed");
		exit(-1);
	}
	else if(pid1 == 0){
		if(pp == 1){ // pipe
			close(pfd[0]);
			dup2(pfd[1], STDOUT_FILENO);
			close(pfd[1]);
			execvp(tokens[0],tokens);
		}
		else if(np == 0){
			if(io == 1){ // i/o redirection
				oldfd = open(ioFilename,(O_RDWR|O_CREAT),0644);
				dup2(oldfd,1);
				close(oldfd);
			}
			execvp(tokens[0],tokens);
		}
		else{ //no delay for "&"
			int childpid = fork();
			if(childpid < 0){
				fprintf(stderr,"Fork failed");
				exit(-1);
			}
			else if(childpid > 0){
				exit(0);
			}
			else{
				execvp(tokens[0],tokens);
			}
		}
	}
	if(pp == 1){
		pid2 = fork();
		if(pid2 == 0){
			close(pfd[1]);
			dup2(pfd[0],STDIN_FILENO);
			close(pfd[0]);
			execvp(secondCommand[0],secondCommand);
		}
	}
	else{
		//if(pp == 1){
			wait(NULL);
		//}
		wait(NULL);
		return 1;
	}
}
#define LINE_SIZE 300
char* readline(){
	int c;
	pos = 0;
	char* line = (char*)malloc(LINE_SIZE*sizeof(char));
	if(!line){
		printf("memory allocation error\n");
		exit(0);
	}
	while(1){
		c = getchar();
		if( c==EOF || c=='\n' ){
			line[pos] = '\0';
			return line;
		}
		else{
			line[pos++] = c;
		}
	}
}
#define LINES_NUM 15
#define LINES_SIZE 20
char** cutline(char* line){
	char** lines = (char**)malloc(LINES_NUM*sizeof(char*));
	
	if(!lines){
		printf("memory allocation error\n");
		exit(0);
	}
	
	for(int i=0;i<LINES_NUM;i++)
		lines[i] = (char*)malloc(LINES_SIZE*sizeof(char));
	
	char* pch;
	pos = 0;

	pch = strtok(line," \t");
	while(pch != NULL){
		strcpy(lines[pos++], pch);
		pch = strtok(NULL, " \t");
	}
	lines[pos] = '\0';
	
	return lines;
}
