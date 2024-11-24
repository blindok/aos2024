#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define MESSAGE_SIZE    64

struct message {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

int msgid;

void handler(int sig) {
    printf("received signal->exit\n");
    msgctl(msgid, IPC_RMID, NULL);
    exit(sig);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int key = ftok(argv[1], 'a');
    if (key == -1) {
        perror("ftok() error");
        return 1;
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget() error");
        return 1;
    }

    int ppid = getpid();

    int pid = fork();
    if (pid > 0) {
        signal(SIGINT, handler);
        struct message m;
        while (1) {
            m.msg_type = pid;
            printf("[1] Parent is goint to send in a sec...\n");
            sleep(rand()%5);
            msgsnd(msgid, &m, sizeof(m.msg_text), 0);
            msgrcv(msgid, &m, sizeof(m.msg_text), ppid, 0);
            printf("[4] Parent received recieved!\n");
        }
    } else if (pid == 0) {
        struct message m;
        while (1) {
            msgrcv(msgid, &m, sizeof(m.msg_text), getpid(), 0);
            printf("[2] Child recieved!\n");
            sleep(rand()%5);
            m.msg_type = ppid;
            printf("[3] Child is goind to send in a sec...\n");
            sleep(rand()%5);
            msgsnd(msgid, &m, sizeof(m.msg_text), 0);
        }
    } else {
        perror("fork() error");
        return 1;
    }

    return 0;
}