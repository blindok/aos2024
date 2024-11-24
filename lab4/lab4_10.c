#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define SIZE 2

int main() {
    int pid = fork();

    if (pid > 0) {
        char* tag = "[parent]";
        char* msg = calloc(sizeof(tag) + SIZE, sizeof(char));
        strcpy(msg, tag);

        char buf[SIZE] = {0};
        while(1) {
            memset(buf, 0, SIZE);
            int readBytes = read(STDIN_FILENO, buf, SIZE);
            write(STDOUT_FILENO, strcat(msg, buf), sizeof(tag) + readBytes);
            strcpy(msg, tag);
        }
        
    } else if (pid == 0) {
        char* tag = "[child]";
        char* msg = calloc(sizeof(tag) + SIZE, sizeof(char));
        strcpy(msg, tag);

        char buf[SIZE] = {0};
        while(1) {
            memset(buf, 0, SIZE);
            int readBytes = read(STDIN_FILENO, buf, SIZE);
            write(STDOUT_FILENO, strcat(msg, buf), sizeof(tag) + readBytes);
            strcpy(msg, tag);
        }
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}