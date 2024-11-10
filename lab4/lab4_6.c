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

    if (mkfifo(argv[1], 0777) == -1) {
        perror("mkfifo() error");
        return 1;
    }

    int fd = open(argv[1], O_WRONLY);
    if (fd < 0) {
        perror("open() error");
        unlink(argv[1]);
        return 1;
    }

    const char *text = "Test Text!";
    write(fd, text, strlen(text));
    close(fd);
    unlink(argv[1]);

    return 0;
}