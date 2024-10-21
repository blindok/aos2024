#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("SON:\n");
        printf("PID: %d\n", getpid());
        printf("PPID %d\n", getppid());
        printf("PGID: %d\n", getpgid(0));
        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        printf("PARENT:\n");
        printf("PID: %d\n", getpid());
        printf("PPID: %d\n", getppid());
        printf("PGID: %d\n", getpgid(0));

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("son process exited with the code: %d\n", WEXITSTATUS(status));
        } else {
            printf("son process exited successfully\n");
        }
    } else {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}