#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define MESSAGE_SIZE    64

int msgid;

struct message {
    long msg_type;
    long pid;
    char msg_text[MESSAGE_SIZE];
};

void handler(int sig) {
    printf("[server] received signal.\n");
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
    printf("[server] generated queue key: %d.\n", key);

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[server] has got queue id: %d.\n", msgid);

    signal(SIGINT, handler);

    struct message request, response;

    while(1) {
        if (msgrcv(msgid, &request, sizeof(request) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv() error");
            continue;
        } else {
            printf("[server] recieved a message of type %ld from client %ld: \"%s\"\n", request.msg_type, request.pid, request.msg_text);
        }

        response.msg_type = request.pid;
        response.pid = 0;
        snprintf(response.msg_text, sizeof(response.msg_text), "hello from server to client %ld!", request.pid);

        if (msgsnd(msgid, &response, sizeof(response) - sizeof(long), 0) == -1) {
            if (errno == EINVAL) {
                fprintf(stderr, "Error: Message queue with ID %d does not exist or has an invalid ID.\n", msgid);
                return 1;
            } else {
                perror("msgsnd() error");
            }
            continue;
        }
        printf("[server] sent a message to client %ld.\n", response.msg_type);
        printf("\n");
    }

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}