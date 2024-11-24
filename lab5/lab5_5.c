#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage format: %s <msgid>\n", argv[0]);
        return 1;
    }

    int msgid = atoi(argv[1]);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        switch (errno) {
            case EINVAL:
                fprintf(stderr, "Error: Message queue with ID %d does not exist or has an invalid ID.\n", msgid);
                break;
            case EPERM:
                fprintf(stderr, "Error: Insufficient permissions to delete the message queue with ID %d.\n", msgid);
                break;
            case EACCES:
                fprintf(stderr, "Error: No access to queue with ID %d.\n", msgid);
                break;
            case EIDRM:
                fprintf(stderr, "Error: Queue with ID %d has already been deleted by another process.\n", msgid);
            default:
                perror("Error deleting the message queue");
        }
        return 1;
    }

    printf("Message queue with ID %d was successfully deleted.\n", msgid);
    return 0;
}