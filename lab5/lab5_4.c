#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct message {
    long msg_type;
    char msg_text[100];
};

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage format: %s <msgid> <msg_type> <number_of_bytes>\n", argv[0]);
        return 1;
    }

    int msgid = atoi(argv[1]);
    long msg_type = atol(argv[2]);
    int num_bytes = atoi(argv[3]);
    printf("Read first %d bytes of a file of type %ld from queue with id: %d\n", num_bytes, msg_type, msgid);

    struct message msg;
    memset(&msg, 0, sizeof(msg));

    int received_bytes = msgrcv(msgid, &msg, num_bytes, msg_type, IPC_NOWAIT | MSG_NOERROR);

    if (received_bytes == -1) {
        if (errno == ENOMSG) {
            printf("No message of type %ld in queue.\n", msg_type);
        } else {
            perror("msgrcv error()");
            return 1;
        }
    } else {
        printf("Recieved message of type %ld: ", msg.msg_type);
        fwrite(msg.msg_text, sizeof(char), received_bytes, stdout); // Вывод первых считанных байтов
        printf("\n");
    }

    return 0;
}