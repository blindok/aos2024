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
    long pid;
    char msg_text[MESSAGE_SIZE];
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file> <sleep delay>\n", argv[0]);
        return 1;
    }

    int key = ftok(argv[1], 'a');
    if (key == -1) {
        perror("ftok() error");
        return 1;
    }
    int pid = getpid();
    printf("[client %d] generated queue key: %d.\n", pid, key);

    int msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[client %d] has got queue id: %d.\n", pid, msgid);

    int delay = atoi(argv[2]);

    struct message request, response;
    for(int i = 1;;++i) {
        request.msg_type = 1;
        request.pid = pid;
        snprintf(request.msg_text, sizeof(request.msg_text), "hello x%d from client %d", i, pid);

        if (msgsnd(msgid, &request, sizeof(request) - sizeof(long), 0) == -1) {
            if (errno == EINVAL) {
                fprintf(stderr, "Error: Message queue with ID %d does not exist or has an invalid ID.\n", msgid);
            } else {
                perror("msgsnd() error");
            }
            return 1;
        }
        printf("[client %d] has sent %d message to server.\n", pid, i);

        if (msgrcv(msgid, &response, sizeof(response) - sizeof(long), pid, 0) == -1) {
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