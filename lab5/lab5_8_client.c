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

int client_msgid;

struct message {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

void handler(int sig) {
    printf("[client %d] received signal.\n", getpid());
    msgctl(client_msgid, IPC_RMID, NULL);
    exit(sig);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server file> <sleep delay>\n", argv[0]);
        return 1;
    }

    int key = ftok(argv[1], 'a');
    if (key == -1) {
        perror("ftok() error");
        return 1;
    }
    int pid = getpid();
    printf("[client %d] generated key: %d.\n", pid, key);

    int server_msgid = msgget(key, 0666);
    if (server_msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[client %d] has got server queue id: %d.\n", pid, server_msgid);

    client_msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (client_msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("[client %d] has got their own queue id: %d.\n", pid, client_msgid);

    signal(SIGINT, handler);

    int delay = atoi(argv[2]);

    struct message request, response;
    for(int i = 1;;++i) {
        request.msg_type = client_msgid;
        snprintf(request.msg_text, sizeof(request.msg_text), "hello x%d from client %d", i, pid);

        if (msgsnd(server_msgid, &request, sizeof(request.msg_text), 0) == -1) {
            if (errno == EINVAL) {
                fprintf(stderr, "Error: Message queue with ID %d does not exist or has an invalid ID.\n", server_msgid);
            } else {
                perror("msgsnd() error");
            }
            msgctl(client_msgid, IPC_RMID, NULL);
            return 1;
        }
        printf("[client %d] has sent %d message to server.\n", pid, i);

        if (msgrcv(client_msgid, &response, sizeof(response.msg_text), 0, 0) == -1) {
            perror("msgrcv error()");
            msgctl(client_msgid, IPC_RMID, NULL);
            return 1;
        } else {
            printf("[client %d] recieved a message of type %ld: \"%s\"\n", pid, response.msg_type, response.msg_text);
        }

        printf("\n");

        sleep(delay);
    }

    msgctl(client_msgid, IPC_RMID, NULL);

    return 0;
}