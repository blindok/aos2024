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

        sleep(5);
        printf("son exits (PPID: %d)\n", getppid());

        exit(EXIT_SUCCESS);
    } else if (pid > 0) {
        printf("PARENT:\n");
        printf("PID: %d\n", getpid());
        printf("PPID: %d\n", getppid());
        printf("PGID: %d\n", getpgid(0));
        printf("parent continues...\n");
    } else {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    sleep(25);
    printf("parent exits\n");

    return EXIT_SUCCESS;
}