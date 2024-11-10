#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <time.h>

int main() {
    int _pipe[2];

    if (pipe(_pipe) == -1) {
        perror("pipe() error");
        return 1;
    }

    int pid = fork();

    if (pid == 0) {
        close(_pipe[1]);

        if (dup2(_pipe[0], STDIN_FILENO) == -1) {
            perror("dup2() error");
            exit(1);
        }

        close(_pipe[0]);

        execlp("cat", "cat", (char*)NULL);
        perror("exec() error");
        exit(1);

    } else if (pid > 0) {
        close(_pipe[0]);

        const char *message = "Hello from parent process!";
        write(_pipe[1], message, strlen(message));
        
        close(_pipe[1]);
        wait(NULL);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}