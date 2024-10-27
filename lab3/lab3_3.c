#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main() {
    signal(SIGCHLD, SIG_IGN);

    for (int i = 0; i < 10; ++i) {
        int pid = fork();
        if (pid > 0) {

        } else if (pid == 0) {
            printf("Child pid: %d\n", getpid());
            sleep(7);
            exit(0);
        } else {
            perror("fork() error");
            return 1;
        }
    }

    sleep(20);

    return 0;
}