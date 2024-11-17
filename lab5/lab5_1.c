#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message {
    long msg_type;
    char msg_text[50];
};

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage format: %s <file>\n", argv[0]);
        return 1;
    }

    struct msqid_ds buf;
    struct message msg;

    int key = ftok("progfile", 'a');
    if (key == -1) {
        perror("ftok() error");
        return 1;
    }
    printf("Generated key: %d\n", key);

    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget() error");
        return 1;
    }
    printf("msgid: %d\n", msgid);

    for (int i = 1; i <= 5; i++) {
        msg.msg_type = i;
        snprintf(msg.msg_text, sizeof(msg.msg_text), "Message Type is %d", i);
        
        if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd() error");
            break;
        }
    }

    if (msgctl(msgid, IPC_STAT, &buf) == -1) {
        perror("msgctl() error");
        return 1;
    }

    printf("\n");
    struct ipc_perm perm = buf.msg_perm;
    printf("Key: %d\n", perm.__key);
    printf("Owner UID: %d\n", perm.uid);
    printf("Owner GID: %d\n", perm.gid);
    printf("Creator UID: %d\n", perm.cuid);
    printf("Creator GID: %d\n", perm.cgid);
    printf("Permissions: %o\n", perm.mode);
    printf("Size: %lu\n", buf.__msg_cbytes);
    printf("Max size: %lu\n", buf.msg_qbytes);
    printf("Messages in queue: %lu\n", buf.msg_qnum);
    printf("PID last sender: %d\n", buf.msg_lspid);
    printf("PID last receiver: %d\n", buf.msg_lrpid);
    printf("Time of last send: %ld\n", buf.msg_stime);
    printf("Time of last read: %ld\n", buf.msg_rtime);
    printf("Time of last change: %ld\n", buf.msg_ctime);

    //msgctl(msgid, IPC_RMID, NULL);
    return 0;
}