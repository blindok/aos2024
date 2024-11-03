#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int _pipe[2]; // child -> parent

    if (pipe(_pipe) == -1) {
        perror("pipe() error");
        return 1;
    }

    int pid = fork();

    if (pid > 0) {
        close(_pipe[1]);

        char buf[128];
        int bytes_read;
        
        while (bytes_read = read(_pipe[0], buf, sizeof(buf) - 1) > 0) {
            printf("Parent received: %s", buf);
        }

        wait(NULL);

        close(_pipe[0]);

    } else if (pid == 0) {
        close(_pipe[0]);

        char buf[128];
        while (fgets(buf, sizeof(buf), stdin) != NULL) {
            write(_pipe[1], buf, strlen(buf));
        }

        close(_pipe[0]);

        exit(0);

    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}