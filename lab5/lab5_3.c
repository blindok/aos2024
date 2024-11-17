#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

struct message {
    long msg_type;
    char msg_text[100];
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage format: %s <msgid> <msg_type>\n", argv[0]);
        return 1;
    }

    int msgid = atoi(argv[1]);
    long msg_type = atol(argv[2]);

    struct message msg;

    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), msg_type, IPC_NOWAIT) == -1) {
        if (errno == ENOMSG) {
            printf("No message of type %ld in queue.\n", msg_type);
        } else {
            perror("msgrcv error()");
            return 1;
        }
    } else {
        printf("Recieved: %s (type: %ld)\n", msg.msg_text, msg.msg_type);
    }

    return 0;
}