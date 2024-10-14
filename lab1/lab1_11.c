#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "usage format: %s <file> [<file> ...]\n", argv[0]);
        return 1;
    }

    struct stat stat_buff;
    size_t max_size = 0;
    char *longest_file = NULL;

    for (int i=1; i < argc; ++i) {
        if (stat(argv[i], &stat_buff) < 0) {
            fprintf(stderr, "Coundn't get stat on \"%s\": %s", argv[i], strerror(errno));
            return 1;
        }
        if (stat_buff.st_size > max_size) {
            max_size = stat_buff.st_size;
            longest_file = argv[i];
        }
    }

    printf("longest file: %s\nsize: %ld\n", longest_file, max_size);

    return 0;
}