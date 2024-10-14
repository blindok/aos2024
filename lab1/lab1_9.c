#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int copy() {
    char buff[512];
    size_t bytes_read;
    size_t total_bytes = 0;
    while ((bytes_read = read(STDIN_FILENO, buff, sizeof(buff))) > 0) {
        if (write(STDOUT_FILENO, buff, bytes_read) != bytes_read) {
            return -1;
        }
        total_bytes += bytes_read;
    }
    return total_bytes;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage format: %s <src file> <dst file>\n", argv[0]);
        return 1;
    }

    char *src_filename = argv[1];
    char *dst_filename = argv[2];

    int fd_dst, fd_src;
    fd_src = open(src_filename, O_RDONLY);
    if (fd_src < 0) {
        perror("Couldn't open the source file");
        return 1;
    }

    struct stat src_stat;
    stat(src_filename, &src_stat);

    if (dup2(fd_src, STDIN_FILENO) < 0) {
        perror("dup2 source error");
        close(fd_src);
        return 1;
    }

    close(fd_src);

    fd_dst = open(dst_filename, O_CREAT | O_WRONLY | O_TRUNC, src_stat.st_mode);
    if (fd_dst < 0) {
        perror("Couldn't open the destination file");
        close(fd_src);
        return 1;
    }

    if (dup2(fd_dst, STDOUT_FILENO) < 0) {
        perror("dup2 destination error");
        close(fd_dst);
        return 1;
    }

    close(fd_dst);

    if (copy() < 0) {
        perror("Coundn't write into the destination file");
    }

    return 0;
}