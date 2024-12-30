#include <stdio.h>
#include "server.h"

#define BUF_SIZE 256

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage format: %s <hostname> <port>\n", argv[0]);
        return 1;
    }

    const char* hostname = argv[1];
    int port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        fprintf(stderr, "[error] failed to create socket\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "[error] incorrect address to establish connection with\n");
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "[error] failed to connect to server %s:%d\n", hostname, port);
        return 1;
    }

    printf("Connect to server %s:%d\n", hostname, port);

    int quantity = 0;
    while (quantity == 0) {
        printf("Enter number of tasks you'd like to solve: ");
        if (scanf("%d", &quantity) <= 0) {
            return 0;
        }
    }

    char buf[BUF_SIZE];
    sprintf(buf, "start %d", quantity);
    if (send(client_socket, buf, strlen(buf), 0) < 0) {
        fprintf(stderr, "[error] failed to send a request to the server\n");
        return 1;
    }

    int result = 0;
    for (int i = 0; i < quantity; ++i) {
        memset(buf, 0, BUF_SIZE);

        if (recv(client_socket, buf, BUF_SIZE, 0) <= 0) {
            fprintf(stderr, "[error] failed to receive a message from the server\n");
            return 1;
        }

        printf("\nTask #%d:\n%s", i + 1, buf);
        scanf("%d", &result);

        sprintf(buf, "%d", result);
        if (send(client_socket, buf, strlen(buf), 0) < 0) {
            fprintf(stderr, "[error] failed to send a response to the server\n");
            return 1;
        }
    }

    if (recv(client_socket, buf, BUF_SIZE, 0) <= 0) {
            fprintf(stderr, "[error] failed to receive a message from the server\n");
            return 1;
        }

        printf("\nTotal time: %s\n", buf);

    printf("End connection with the server\n");
    close(client_socket);

    return 0;
}