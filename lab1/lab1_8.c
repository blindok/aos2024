#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int copy(int fd_src, int fd_dst) {
    char buff[512];
    size_t bytes_read;
    size_t total_bytes = 0;
    while ((bytes_read = read(fd_src, buff, sizeof(buff))) > 0) {
        if (write(fd_dst, buff, bytes_read) != bytes_read) {
            return -1;
        }
        total_bytes += bytes_read;
    }
    return total_bytes;
}

int main(int argc, char* argv[]) {

    char *src_filename;
    char *dst_filename;

    if (argc == 1) {
        src_filename = NULL;
        dst_filename = NULL;
    } else if (argc == 3) {
        src_filename = argv[1];
        dst_filename = argv[2];
    } else {
        fprintf(stderr, "usage format: %s <src file> <dst file>\n", argv[0]);
        return 1;
    }

    int fd_dst, fd_src;
    if (src_filename == NULL) {
        fd_src = STDIN_FILENO;
    } else {
        fd_src = open(src_filename, O_RDONLY);
        if (fd_src < 0) {
            perror("Couldn't open the source file");
            return 1;
        }
    }

    struct stat src_stat;
    stat(src_filename, &src_stat);

    if (dst_filename == NULL) {
        fd_dst = STDOUT_FILENO;
    } else {
        fd_dst = open(dst_filename, O_CREAT | O_WRONLY | O_TRUNC, src_stat.st_mode);
        if (fd_dst < 0) {
            perror("Couldn't open the destination file");
            close(fd_src);
            return 1;
        }
    }

    if (copy(fd_src, fd_dst) < 0) {
        perror("Coundn't write into the destination file");
    }

    close(fd_src);
    close(fd_dst);

    return 0;
}