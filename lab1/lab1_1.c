#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char* file_path = argv[1];
    int fd = open(file_path, O_RDONLY);
    printf("fd = %d\n", fd);

    if (fd == -1) {
        printf("Error: %d\n", errno);
        printf("Error: %s\n", strerror(errno));
        perror("Couldn't open the file");
    } else {
        printf("File is opened successfully\n");
        close(fd);
    }
    
    return 0;
}