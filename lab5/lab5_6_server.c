#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#define MESSAGE_SIZE    64

struct message {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server file>\n", argv[0]);
        return 1;
    }

    int key = ftok(argv[1], 'a');
    if (key == -1) {
        perror("ftok() error");
        return 1;
    }
    printf("[server] generated key: %d.\n", key);

    int server_msgid = msgget(key, 0666 | IPC_CREAT);
    if (server_msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[server] has got queue id: %d.\n", server_msgid);

    struct message request, response;

    while(1) {
        if (msgrcv(server_msgid, &request, sizeof(request.msg_text), 0, 0) == -1) {
            perror("msgrcv() error");
            continue;
        } else {
            printf("[server] recieved a message of type %ld: \"%s\"\n", request.msg_type, request.msg_text);
        }

        response.msg_type = 1;
        snprintf(response.msg_text, sizeof(response.msg_text), "hello from parent to queue %ld!", request.msg_type);

        if (msgsnd(request.msg_type, &response, sizeof(response.msg_text), 0) == -1) {
            perror("msgsnd() error");
            continue;
        }
        printf("[server] sent a message to client with queue %ld.\n", request.msg_type);
        printf("\n");
    }

    msgctl(server_msgid, IPC_RMID, NULL);

    return 0;
}