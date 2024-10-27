#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int pid = fork();

    if (pid > 0) {

        sleep(1);
        kill(pid, SIGUSR1);
        //kill(pid, SIGCHLD);

        int status = 0;
        wait(&status);   

        if (WIFEXITED(status)) {
            printf("child exited with exit code: %d\n", WEXITSTATUS(status));
        } else if WIFSIGNALED(status) {
            printf("child exited with signal: %d (exit code: %d)\n", WTERMSIG(status), WEXITSTATUS(status));
        } else {
            printf("error\n");
        }

    } else if (pid == 0) {
        pause();
        exit(20);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}