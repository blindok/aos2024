#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int pid = fork();

    if (pid > 0) {
        int status = 0;
        wait(&status);

        if (WIFEXITED(status)) {
            printf("child exited with exit code: %d\n", WEXITSTATUS(status));
        } else {
            printf("error\n");
        }

    } else if (pid == 0) {
        for (int i = 0; i < 1000; ++i) {
            printf("%d\n", i);
            for (int j = 0; j < 1000; ++j) {
                for (int k = 0; k < 1000; ++k);
            }
        }
        exit(42);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}