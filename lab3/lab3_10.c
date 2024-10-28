#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void print_sigset(sigset_t *set) {
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(set, i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

void handler(int sig) {
    printf("Handler: %d\n", sig);

    sigset_t block_set, old_set, cur_set;
    sigemptyset(&block_set);
    sigemptyset(&cur_set);
    sigaddset(&block_set, SIGINT);
    sigaddset(&block_set, SIGQUIT);
    sigprocmask(SIG_BLOCK, &block_set, &old_set);

    for (int i = 0; i < 10; ++i) {
        sigpending(&cur_set);
        printf("handler doing smth... %d\n", i);
        print_sigset(&cur_set);
        sleep(3);
    }

    sigprocmask(SIG_SETMASK, &old_set, NULL);
}

int main() {

    int pid = fork();

    if (pid > 0) {

        sleep(2);
        kill(pid, SIGUSR1);

        int code;
        wait(&code);

        if (WIFEXITED(code)) {
            printf("child exited with exit code: %d\n", WEXITSTATUS(code));
        } else if (WIFSIGNALED(code)) {
            printf("child exited with signal: %d (exit code: %d)\n", WTERMSIG(code), WEXITSTATUS(code));
        } else {
            printf("error\n");
        }

    } else if (pid == 0) {
        printf("child: %d\n", getpid());
        signal(SIGUSR1, handler);
        //pause();
        // sigset_t sigset;
        // sigemptyset(&sigset);
        // sigaddset(&sigset, SIGINT);
        // sigprocmask(SIG_BLOCK, &sigset, NULL);

        for (int i = 0; i < 6; ++i) {
            printf("child doing smth... %d\n", i);
            sleep(5);
        }

        exit(0);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}
