#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "format: %s <file>\n", argv[0]);
        return 1;
    }

    struct stat buff;

    if (stat(argv[1], &buff) < 0) {
        perror("stat error");
        return 1;
    }

    printf("File type: %s\n", buff.st_mode & S_IFDIR ? "directive" : "file");
    printf("Access right: %o\n", buff.st_mode);

    struct passwd *pw = getpwuid(buff.st_uid);
    printf("Owner: %s (id=%u)\n", pw ? pw->pw_name : "unknown", buff.st_uid);
    struct group *gr = getgrgid(buff.st_gid);
    printf("Group: %s (id=%u)\n", gr ? gr->gr_name : "unknown", buff.st_gid);

    printf("Size: %ld\n", buff.st_size);

    printf("Device ID: %lu\n", buff.st_dev);
    printf("Device ID for special file: %lu\n", buff.st_rdev);
    printf("Inode: %lu\n", buff.st_ino);
    printf("Number of hard links: %lu\n", buff.st_nlink);
    printf("I/O block size: %lu\n", buff.st_blksize);
    printf("Blocks: %lu\n", buff.st_blocks);
    

    char time_str[256];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&buff.st_mtime));
    printf("Last time modification: %s\n", time_str);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&buff.st_atime));
    printf("Last time access: %s\n", time_str);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&buff.st_ctime));
    printf("Last time status change: %s\n", time_str);


    // printf("Last time access: %ld\n", buff.st_atim.tv_sec);
    // printf("Last time modification: %ld\n", buff.st_mtim.tv_sec);
    // printf("Last time status change: %ld\n", buff.st_ctim.tv_sec);

    return 0;
}