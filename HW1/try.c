#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int forkExecProc (char *prog, char **arg_list);
int main (int argc, char *argv[])
{
    char **prog_list = {"prog1","prog2", "prog3"};

    for (int i=0; i<3; i++) {
        char **arg_list = {prog_list[i], NULL};
        forkExecProc (prog_list[i], arg_list);
    }

    while (1) {
        sleep (2);
    }
    
    return 0;
}

int forkExecProc (char *prog, char **arg_list)
{
    pid_t child;

    /* check if fork fail that first child is not created */
    if ((child = fork ())< 0) {
        fprintf (stderr, "fork error");
    } else if (child == 0) {        /* run into first child */
        fprintf (stdout, "fork to execute : [%s]\n", arg_list);
        /* replaces the current process image with a new process image */
        execvp(prog, arg_list);

        /* if execvp() is return, mean that on error */
        fprintf(stderr, "execvp error");
        exit(0);
    }

    return child;
}
