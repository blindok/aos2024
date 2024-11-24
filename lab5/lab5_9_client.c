#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MESSAGE_SIZE    64

struct message {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file> <sleep delay>\n", argv[0]);
        return 1;
    }

    int request_key = ftok(argv[1], 'a');
    if (request_key == -1) {
        perror("ftok() error");
        return 1;
    }
    int pid = getpid();
    printf("[client %d] generated request-queue key: %d.\n", pid, request_key);

    int response_key = ftok(argv[1], 'b');
    if (response_key == -1) {
        perror("ftok() error");
        return 1;
    }
    printf("[client %d] generated response-queue key: %d.\n", pid, response_key);

    int request_msgid = msgget(request_key, 0666);
    if (request_msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[client %d] has got request-queue id: %d.\n", pid, request_msgid);

    int response_msgid = msgget(response_key, 0666);
    if (response_msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[client %d] has got response-queue id: %d.\n", pid, response_msgid);

    int delay = atoi(argv[2]);

    struct message request, response;
    for(int i = 1;;++i) {
        request.msg_type = pid;
        snprintf(request.msg_text, sizeof(request.msg_text), "hello x%d from client %d", i, pid);

        if (msgsnd(request_msgid, &request, sizeof(request.msg_text), 0) == -1) {
            if (errno == EINVAL) {
                fprintf(stderr, "Error: Message queue with ID %d does not exist or has an invalid ID.\n", request_msgid);
            } else {
                perror("msgsnd() error");
            }
            return 1;
        }
        printf("[client %d] has sent %d message to server.\n", pid, i);

        if (msgrcv(response_msgid, &response, sizeof(response.msg_text), pid, 0) == -1) {
            perror("msgrcv error()");
            return 1;
        } else {
            printf("[client %d] recieved a message of type %ld: \"%s\"\n", pid, response.msg_type, response.msg_text);
        }

        printf("\n");

        sleep(delay);
    }

    return 0;
}