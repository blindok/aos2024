#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int pid = fork();

    if (pid > 0) {
        int fd = open(argv[1], O_CREAT | O_WRONLY, 0666);
        if (fd < 0) {
            perror("parent failed to create/open file");
            kill(pid, 9);
            return 1;
        }

        struct flock lock = {0};
        lock.l_type = F_WRLCK;
        lock.l_pid = getpid();
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        
        fcntl(fd, F_SETLK, &lock);

        sleep(2);

        const char *messages[] = {"1. Hello!\n", "2. I'm your parent!\n", "3. Bye!\n"};
        for (int i = 0; i < 10; ++i) {
            write(fd, messages[i%3], strlen(messages[i%3]));
        }

        struct flock unlock = {0};
        lock.l_type = F_UNLCK;
        lock.l_pid = getpid();
        fcntl(fd, F_SETLK, &lock);

        close(fd);
        wait(NULL);

    } else if (pid == 0) {

        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("child failed to open file");
            exit(1);
        }

        sleep(1);

        char buf[128] = {0};
        int bytesRead = 1;

        while(bytesRead = read(fd, buf, sizeof(buf))) {
            printf("%s", buf);
        }

        close(fd);
        exit(0);

    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}