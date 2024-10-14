#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "format: %s <file name> <access rights>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];
    int mode = strtol(argv[2], NULL, 8);

    int fd = open(file_name, O_CREAT | O_WRONLY, mode);
    if (fd == -1) {
        perror("Couldn't open the file");
        return 1;
    }

    char* messages[] = {"     _\n", "   _| |\n", " _| | |\n", "| | | |\n", "| | | | __\n",
                        "| | | |/  \\\n", "|       /\\ \\\n", "|      /  \\/\n", "|      \\  /\\\n",
                        "|       \\/ /\n", " \\        /\n", "  |     /\n", "  |    |\n"};
    for (int i=0; i < 13; ++i) {
        if (write(fd, messages[i], strlen(messages[i])) == -1) {
            perror("Couldn't write into the file");
            close(fd);
            return 1;
        }
    }

    if (close(fd) == -1) {
        //printf("Error %d: %s\n", errno, strerror(errno));
        perror("Couldn't close the file");
        return 1;
    }
 
    fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        //printf("Error %d: %s\n", errno, strerror(errno));
        perror("Couldn't open the file the second time");
        return 1;
    }

    size_t cnt = 512;
    char buff[cnt];
    size_t bytes_read = read(fd, buff, cnt);
    if (bytes_read < 0) {
        //printf("Error %d: %s\n", errno, strerror(errno));
        perror("Couldn't read the file");
    } else {
        printf("Text from the file \"%s\":\n%s", file_name, buff);
    }

    if (close(fd) == -1) {
        //printf("Error %d: %s\n", errno, strerror(errno));
        perror("Couldn't close the file");
        return 1;
    }

    fd = open(file_name, O_RDWR);
    printf("FD: %d\n", fd);
    //printf("Error %d: %s\n", errno, strerror(errno));
    perror("Couldn't open the file the third time");

    if (fd > 0) {
        if (close(fd) == -1) {
            //printf("Error %d: %s\n", errno, strerror(errno));
            perror("Couldn't close the file");
            return 1;
        }
    }

    return 0;
}