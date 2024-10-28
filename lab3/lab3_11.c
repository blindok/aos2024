#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void handler(int sig) {
    printf("handler %d\n", getpid());
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    int pids[5];

    for (int i = 0; i < 5; ++i) {
        int pid = fork();
        if (pid > 0) {
            printf("Parent (%d) created child %d\n", getpid(), pid);
            pids[i] = pid;
            sleep(2);
        } else if (pid == 0) {
            sigset_t block_set, empty_set;
            sigemptyset(&block_set);
            sigemptyset(&empty_set);
            sigaddset(&block_set, SIGUSR1);
            sigprocmask(SIG_BLOCK, &block_set, NULL);

            printf("child %d is waiting...\n", getpid());
            sigsuspend(&empty_set);
            printf("child %d goes on..\n", getpid());

            exit(0);
        }
    }

    printf("Parent is free...\n");
    kill(0, SIGUSR1);

    for (int i = 0; i < 5; ++i) {
        wait(0);
    }

    return 0;
}