#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
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
	
	int np = 0, io = 0;
	char* ioFilename;
	FILE *fptr;
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
		fptr = freopen(ioFilename,"w",stdout);
		if(!fptr){
			printf("Fail to create file\n");
			exit(0);
		}
		tokens[pos-2] = '\0';
	}

	pid_t pid;
	pid = fork();
	
	if(pid<0){
		fprintf(stderr,"Fork failed");
		exit(-1);
	}
	else if(pid == 0){
		if(np == 0)
			execvp(tokens[0],tokens);
		else{
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
	else{
		wait(NULL);
		if(io == 1){
			fclose(fptr);
			freopen("/dev/tty","a",stdout); //redirect io to screen
		}
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
