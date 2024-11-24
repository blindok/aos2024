#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define SIZE 2

void set_lock(int fd, int lock_type) {
    struct flock lock;
    lock.l_type = lock_type;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    fcntl(fd, F_SETLK, &lock) == -1;
}

int main() {
    int pid = fork();

    if (pid > 0) {
        sleep(1);
        char* tag = "[parent]";
        char* msg = calloc(sizeof(tag) + SIZE, sizeof(char));
        strcpy(msg, tag);

        char buf[SIZE] = {0};
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        while (1) {
            lock.l_type = F_WRLCK;

            while (fcntl(STDOUT_FILENO, F_SETLK, &lock) == -1);

            int readBytes = 1;
            while(readBytes = read(STDIN_FILENO, buf, SIZE)) {
                write(STDOUT_FILENO, strcat(msg, buf), sizeof(tag) + readBytes);
                strcpy(msg, tag);
                if (buf[readBytes-1] == '\n') {
                    break;
                }
            }

            lock.l_type = F_UNLCK;
            fcntl(STDOUT_FILENO, F_SETLK, &lock);
            sleep(1);
        }
        
    } else if (pid == 0) {
        char* tag = "[child]";
        char* msg = calloc(sizeof(tag) + SIZE, sizeof(char));
        strcpy(msg, tag);

        char buf[SIZE] = {0};
        
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;

        while (1) {
            lock.l_type = F_WRLCK;

            while (fcntl(STDOUT_FILENO, F_SETLK, &lock) == -1);

            int readBytes = 1;
            while(readBytes = read(STDIN_FILENO, buf, SIZE)) {
                
                write(STDOUT_FILENO, strcat(msg, buf), sizeof(tag) + readBytes);
                strcpy(msg, tag);
                if (buf[readBytes - 1] == '\n') {
                    break;
                }
            }
            lock.l_type = F_UNLCK;
            fcntl(STDOUT_FILENO, F_SETLK, &lock);
            sleep(1);
        }
        
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}