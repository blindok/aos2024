#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "format: %s <executable_name> [<arg1> <arg2> ...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execvp() failed");
        exit(1);
    } else if (pid > 0) {
        printf("parent (PID: %d)\n", getpid());
        for (int i = 0; i < argc; i++) {
            printf("arg%d: %s\n", i, argv[i]);
        }

        printf("Parent's enviroment:\n");
        for (char **env = environ; *env != NULL; env++) {
            printf("%s\n", *env);
        }

        wait(NULL);
        printf("Parent exits\n");
    } else {
        perror("fork() failed");
        return 1;
    }

    return 0;
}