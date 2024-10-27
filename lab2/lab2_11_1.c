#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    system(argv[1]);
    execlp("ls", "ls", "-la", (char*)NULL);
    return 0;
}