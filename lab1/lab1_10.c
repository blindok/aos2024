#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage format: %s <src file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Couldn't open the source file");
        return 1;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    if (file_size < 0) {
        perror("Couldn't define the size of the file");
        close(fd);
        return 1;
    }

    char s = '\0';
    do {
        read(fd, &s, 1);
        printf("%c", s);
    } while(lseek(fd, -2, SEEK_CUR) >= 0);
    printf("\n");

    close(fd);

    return 0;
}