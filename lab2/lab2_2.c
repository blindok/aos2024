#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {

    pid_t pid = fork();

    if (pid == 0) {
        printf("i'm the son (PID: %d)\n", getpid());
        printf("my parent's PID: %d\n", getppid());
    } else if (pid > 0) {
        printf("i'm the parent (PID: %d)\n", getpid());
        printf("my son's PID: %d\n", pid);
        wait(NULL);
    } else {
        perror("fork failed");
        return 1;
    }

    return 0;
}