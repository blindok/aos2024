#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int sig) {
    static int cnt = 0;
    printf("[handler %d] signal %d received!\n", cnt, sig);
    if (cnt > 2)
        signal(sig, SIG_DFL);
    ++cnt;
}

void action(int sig, siginfo_t *, void *) {
    static int cnt = 0;
    printf("[action %d] signal %d received!\n", cnt, sig);
    if (cnt > 2)
        signal(sig, SIG_DFL);
    ++cnt;
}

int main(int argc, char* argv[]) {
    struct sigaction sa;

    sa.sa_sigaction = action;
    sa.sa_handler = handler;
    sigaction(SIGINT, &sa, NULL);

    while (1)
        sleep(1);
    
    return 0;
}