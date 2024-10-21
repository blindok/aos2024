#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char* argv[]) {

    printf("child (PID: %d)\n", getpid());
    printf("PPID: %d):\n", getppid());

    for (int i = 0; i < argc; i++) {
        printf("arg%d: %s\n", i, argv[i]);
    }

    printf("Child's enviroment:\n");
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }

    return 0;
}