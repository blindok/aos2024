#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void handler(int sig) {
    printf("Child recieved alarm: %d\n", sig);
}

int main() {

    int pid = fork();

    if (pid > 0) {
        int status = -1;
        wait(&status);

        if (WIFEXITED(status)) {
            printf("child exited with exit code: %d\n", WEXITSTATUS(status));
        } else if WIFSIGNALED(status) {
            printf("child exited with signal: %d (exit code: %d)\n", WTERMSIG(status), WEXITSTATUS(status));
        } else {
            printf("error\n");
        }
    } else if (pid == 0) {
        signal(SIGALRM, handler);
        alarm(3);
        pause();
        exit(3);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}