#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define MESSAGE_SIZE    64

int request_msgid;
int response_msgid;

struct message {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

void handler(int sig) {
    printf("[server] received signal.\n");
    msgctl(request_msgid, IPC_RMID, NULL);
    msgctl(response_msgid, IPC_RMID, NULL);
    exit(sig);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int request_key = ftok(argv[1], 'a');
    if (request_key == -1) {
        perror("ftok() error");
        return 1;
    }
    printf("[server] generated request-queue key: %d.\n", request_key);

    int response_key = ftok(argv[1], 'b');
    if (response_key  == -1) {
        perror("ftok() error");
        return 1;
    }
    printf("[server] generated response-queue key: %d.\n", response_key);

    request_msgid = msgget(request_key, 0666 | IPC_CREAT);
    if (request_msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[server] has got request-queue id: %d.\n", request_msgid);

    response_msgid = msgget(response_key, 0666 | IPC_CREAT);
    if (response_msgid == -1) {
        perror("msgget() error");
        msgctl(request_msgid, IPC_RMID, NULL);
        return 1;
    }
    printf("[server] has got response-queue id: %d.\n", response_msgid);

    signal(SIGINT, handler);

    struct message request, response;

    while(1) {
        if (msgrcv(request_msgid, &request, sizeof(request.msg_text), 0, 0) == -1) {
            perror("msgrcv() error");
            continue;
        } else {
            printf("[server] recieved a message of type %ld: \"%s\"\n", request.msg_type, request.msg_text);
        }

        response.msg_type = request.msg_type;
        snprintf(response.msg_text, sizeof(response.msg_text), "hello from server to client %ld!", request.msg_type);

        if (msgsnd(response_msgid, &response, sizeof(response.msg_text), 0) == -1) {
            if (errno == EINVAL) {
                fprintf(stderr, "Error: Message queue with ID %d does not exist or has an invalid ID.\n", response_msgid);
                return 1;
            } else {
                perror("msgsnd() error");
            }
            continue;
        }
        printf("[server] sent a message to client %ld.\n", request.msg_type);
        printf("\n");
    }

    msgctl(request_msgid, IPC_RMID, NULL);
    msgctl(response_msgid, IPC_RMID, NULL);

    return 0;
}