#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void closePipes(int pipefd[4]) {
    close(pipefd[0]);
    close(pipefd[1]);
    close(pipefd[2]);
    close(pipefd[3]);
}

int main(void) {
    int pipefd[4];
    pid_t cpid;

    if (pipe(pipefd) == -1) {
        perror("pipe1");
        exit(EXIT_FAILURE);
    };
    if (pipe(pipefd + 2) == -1) {
        perror("pipe2");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork1");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {
        dup2(pipefd[2], STDIN_FILENO); //read output from ls in second pipe

        closePipes(pipefd);

        execlp("wc", "wc", "-l", NULL);
        exit(EXIT_SUCCESS);

    } else {

        cpid = fork();
        if (cpid == -1) {
            perror("fork2");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) {
            dup2(pipefd[0], STDIN_FILENO); //read Output from ls in first pipe
            dup2(pipefd[3], STDOUT_FILENO); //redirect Output from grep to second pipe
            
            closePipes(pipefd);

            execlp("grep", "grep", "-v", "lab", NULL);
            exit(EXIT_SUCCESS);
        } else {
            dup2(pipefd[1], STDOUT_FILENO);
            closePipes(pipefd);
            execlp("ls", "ls", NULL);
            wait(NULL);
            exit(EXIT_SUCCESS);
        }

    }
}
