#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int pipe_1[2], pipe_2[2]; // 1: parent -> child         2: child -> parent

    if (pipe(pipe_1) == -1) {
        perror("pipe() error");
        return 1;
    }

    if (pipe(pipe_2) == -1) {
        perror("pipe() error");
        close(pipe_1[0]);
        close(pipe_1[1]);
        return 1;
    }

    int pid = fork();

    if (pid > 0) {
        close(pipe_1[0]);
        close(pipe_2[1]);

        const char* message = "Hi from parent!\n";
        write(pipe_1[1], message, strlen(message));

        char buf[64];
        read(pipe_2[0], buf, sizeof(buf));
        printf("Parent received: %s\n", buf);

        wait(NULL);

        close(pipe_1[1]);
        close(pipe_2[0]);

    } else if (pid == 0) {
        close(pipe_1[1]);
        close(pipe_2[0]);

        char buf[64];
        read(pipe_1[0], buf, sizeof(buf));
        printf("Child received: %s\n", buf);

        const char* message = "Hi from child!\n";
        write(pipe_2[1], message, strlen(message));

        close(pipe_1[0]);
        close(pipe_2[1]);

        exit(0);

    } else {
        perror("fork() error");
    }

    return 0;
}