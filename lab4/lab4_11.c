#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define SIZE 50

void set_lock(int fd, int lock_type) {
    struct flock lock;
    lock.l_type = lock_type;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    while(fcntl(fd, F_SETLK, &lock) == -1) ;
}

int main() {
    int pid = fork();

    if (pid > 0) {
        char* tag = "[parent]";
        char* msg = calloc(sizeof(tag) + SIZE, sizeof(char));
        strcpy(msg, tag);

        char buf[SIZE] = {0};
        while(1) {
            memset(buf, 0, SIZE);

            set_lock(STDOUT_FILENO, F_WRLCK);

            int readBytes = read(STDIN_FILENO, buf, SIZE);
            write(STDOUT_FILENO, strcat(msg, buf), sizeof(tag) + readBytes);
            strcpy(msg, tag);

            set_lock(STDOUT_FILENO, F_UNLCK);
        }
        
    } else if (pid == 0) {
        char* tag = "[child]";
        char* msg = calloc(sizeof(tag) + SIZE, sizeof(char));
        strcpy(msg, tag);

        char buf[SIZE] = {0};
        while(1) {
            memset(buf, 0, SIZE);

            set_lock(STDOUT_FILENO, F_WRLCK);

            int readBytes = read(STDIN_FILENO, buf, SIZE);
            write(STDOUT_FILENO, strcat(msg, buf), sizeof(tag) + readBytes);
            strcpy(msg, tag);

            set_lock(STDOUT_FILENO, F_UNLCK);
        }
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}