#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 *  
 * Code adapted from man 2 pipe
 * 
 */

int main(void){

    int pipefd[2];
    pid_t cpid;

    // pipe1[0] refers to the read end of the pipe
    // pipe1[1] refers to the write end of the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
   
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //child process will execute
    if (cpid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execlp("wc", "wc", "-l", NULL);
        exit(EXIT_SUCCESS);
    } 
    // or parent process
    else {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execlp("ls", "ls", NULL);
        wait(NULL);
        
    }
    return EXIT_SUCCESS;
}