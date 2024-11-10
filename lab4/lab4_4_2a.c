#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    int readBytes;

    do {
        char buf[128] = {0};
        readBytes = read(STDIN_FILENO, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, readBytes);
    } while(readBytes);
}