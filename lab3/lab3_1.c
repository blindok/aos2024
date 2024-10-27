#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig) {
    static int cnt = 0;
    printf("[%d] signal %d received!\n", cnt, sig);
    if (cnt > 2)
        signal(sig, SIG_DFL);
    ++cnt;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handler);

    while (1)
        sleep(1);
    
    return 0;
}