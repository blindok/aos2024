
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>

int main() {
    int pipefd[2];
    char buffer[128];
    

    if (pipe(pipefd) == -1) {
        perror("pipe() error");
        return 1;
    }

    int pid = fork();

    if (pid > 0) {
        close(pipefd[1]);

        fd_set readfds;

        FD_ZERO(&readfds);
        FD_SET(pipefd[0], &readfds);

        struct timeval timeout;

        while (1) {
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            int ready = select(pipefd[0] + 1, &readfds, NULL, NULL, &timeout);

            if (ready == -1) {
                perror("select() error");
                exit(1);
            } else if (ready == 0) {
                printf("timeout\n");
                kill(pid, 9);
                break;
            }

            int bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);

            if (bytesRead == 0) {
                break;
            } else if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                printf("Parent received: %s", buffer);
            }
        }

        close(pipefd[0]);
        wait(NULL);
    } else if (pid == 0) {
        close(pipefd[0]);

        while (fgets(buffer, sizeof(buffer), stdin)) {
            write(pipefd[1], buffer, strlen(buffer));
        }

        close(pipefd[1]);

        exit(0);
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}