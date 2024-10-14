#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "format: %s <file name> \n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];

    int fd = open(file_name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Couldn't open the file");
        return 1;
    }

    char* messages[] = {"first string\n", "second string\n"};

    if (write(fd, messages[0], strlen(messages[0])) < 0) {
        perror("1. Couldn't write into the file");
        close(fd);
        return 1;
    }

    if (lseek(fd, 30, SEEK_SET) < 0) {
        perror("lseek error");
        close(fd);
        return 1;
    }

    if (write(fd, messages[1], strlen(messages[1])) < 0) {
        perror("2. Couldn't write into the file");
        close(fd);
        return 1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek error");
        close(fd);
        return 1;
    }

    size_t cnt = 512;
    char buff[cnt];
    size_t bytes_read = 0;

    bytes_read = read(fd, buff, cnt);
    if (bytes_read < 0) {
        perror("Couldn't read the file");
    } else {
        printf("[seek=0]Text from the file \"%s\":\n%s", file_name, buff);
    }

    if (lseek(fd, 30, SEEK_SET) < 0) {
        perror("lseek error");
        close(fd);
        return 1;
    }

    bytes_read = read(fd, buff, cnt);
    if (bytes_read < 0) {
        perror("Couldn't read the file");
    } else {
        printf("\n[seek=6]Text from the file \"%s\":\n%s", file_name, buff);
    }

    if (close(fd) == -1) {
        perror("Couldn't close the file");
        return 1;
    }

    return 0;
}