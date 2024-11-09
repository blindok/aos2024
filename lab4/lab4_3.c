#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <time.h>

int main() {
    int pipePtoCh[2], pipeChtoP[2]; // 1: parent -> child         2: child -> parent

    if (pipe(pipePtoCh) == -1) {
        perror("pipe() error");
        return 1;
    }

    if (pipe(pipeChtoP) == -1) {
        perror("pipe() error");
        close(pipePtoCh[0]);
        close(pipePtoCh[1]);
        return 1;
    }

    int pid = fork();

    if (pid == 0) {
        close(pipePtoCh[1]);
        close(pipeChtoP[0]);

        int readBytes;
        char buf[128] = {0};

        while(1) {
            memset(buf, 0, 128);
            readBytes = read(STDIN_FILENO, buf, sizeof(buf));
            write(pipeChtoP[1], buf, readBytes);

            memset(buf, 0, 128);
            readBytes = read(pipePtoCh[0], buf, sizeof(buf));
            if (readBytes > 0) {
                printf("Child received: %s", buf);
            }
        }

        close(pipePtoCh[0]);
        close(pipeChtoP[1]);

        exit(0);
    } else if (pid > 0){
        close(pipePtoCh[0]);
        close(pipeChtoP[1]);

        int readBytes;

        do {
            char buf[128] = {0};
            readBytes = read(pipeChtoP[0], buf, sizeof(buf));
            if (readBytes > 0) {
                printf("Parent received: %s", buf);
                write(pipePtoCh[1], buf, readBytes);
            }
        } while(readBytes);

        close(pipePtoCh[1]);
        close(pipeChtoP[0]);

        wait(NULL);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}