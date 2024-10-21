#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>


#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "format: %s <input_file>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        int input_fd = open(argv[1], O_RDONLY);
        if (input_fd < 0) {
            perror("Child coudln't open the file");
            return 1;
        }

        char output_file[100] = "child.txt";
        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (output_fd == -1) {
            perror("Child couldn't create the file");
            exit(1);
        }

        char buffer[BUF_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(input_fd, buffer, BUF_SIZE)) > 0) {
            write(output_fd, buffer, bytes_read);
        }

        close(output_fd);
        close(input_fd);

        output_fd = open(output_file, O_RDONLY);
        if (output_fd == -1) {
            perror("Child couldn't open the file");
            return 1;
        }
        while ((bytes_read = read(output_fd, buffer, BUF_SIZE)) > 0) {
            write(1, buffer, bytes_read);
        }
        close(output_fd);

        exit(0);
    } else if (pid > 0) {

        int input_fd = open(argv[1], O_RDONLY);
        if (input_fd < 0) {
            perror("Parent coudln't open the file");
            return 1;
        }

        char output_file[100] = "parent.txt";
        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (output_fd == -1) {
            perror("Parent couldn't create the file");
            exit(1);
        }

        char buffer[BUF_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(input_fd, buffer, BUF_SIZE)) > 0) {
            write(output_fd, buffer, bytes_read);
        }

        close(output_fd);
        close(input_fd);

        wait(NULL);

        output_fd = open(output_file, O_RDONLY);
        if (output_fd == -1) {
            perror("Parent couldn't open the file");
            return 1;
        }
        while ((bytes_read = read(output_fd, buffer, BUF_SIZE)) > 0) {
            write(1, buffer, bytes_read);
        }
        close(output_fd);

        exit(0);
    } else {
        perror("fork() failed");
        return 1;
    }
}