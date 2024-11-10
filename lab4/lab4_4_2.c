#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <time.h>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file to get executed>", argv[0]);
        return 1;
    }

    int _pipe[2];

    if (pipe(_pipe) == -1) {
        perror("pipe() error");
        return 1;
    }

    int pid = fork();

    if (pid == 0) {
        close(_pipe[0]);

        if (dup2(_pipe[1], STDOUT_FILENO) == -1) {
            perror("dup2() error");
            exit(1);
        }

        close(_pipe[1]);

        execl(argv[1], argv[1], NULL);
        perror("exec() error");
        exit(1);
    } else if (pid > 0) {
        close(_pipe[1]);

        int readBytes;
        char buf[128] = {0};
        do {
            memset(buf, 0, sizeof(buf));
            readBytes = read(_pipe[0], buf, sizeof(buf));
            printf("Parent received: %s", buf);
        } while(readBytes);

        close(_pipe[0]);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}