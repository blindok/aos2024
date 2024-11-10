#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open() error");
        unlink(argv[1]);
        return 1;
    }

    char buf[64] = {0};
    read(fd, buf, sizeof(buf));

    printf("read from the channel: %s", buf);

    close(fd);
    unlink(argv[1]);

    return 0;
}