#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pid1, pid2;
    int _pipe[2];

    if (pipe(_pipe) == -1) {
        perror("pipe() error");
        return 1;
    }

    pid1 = fork();

    if (pid1 == 0) {
        close(_pipe[0]);

        if (dup2(_pipe[1], STDOUT_FILENO) == -1) {
            perror("dup() pid1 error");
            exit(1);
        } 

        close(_pipe[1]);

        execlp("who", "who", (char*)NULL);
        perror("exec() pid1 error");
        exit(1);
    } else if (pid1 < 0) {
        perror("fork() pid1 error");
        return 1;
    }

    pid2 = fork();

    if (pid2 == 0) {
        close(_pipe[1]);

        if (dup2(_pipe[0], STDIN_FILENO) == -1) {
            perror("dup() pid2 error");
            exit(1);
        }

        close(_pipe[0]);

        execlp("wc", "wc", "-l", (char*)NULL);
        perror("exec() pid2 error");
        exit(1);
    } else if (pid2 < 0) {
        perror("fork() pid2 error");
        kill(pid1, 9);
        return 1;
    }

    close(_pipe[1]);
    close(_pipe[0]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}