#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "socket.h"

#define MAX 128

void *chat(int sockfd);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        perror("No enough arguments\n");
        exit(EXIT_FAILURE);
    }
    int port = strtol(argv[1], NULL, 10);

    socket_t sock;

    sock.sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //difference, only use for client

    if (connect(sock.sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("connection with server failed.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("connected to server.\n");
    }

    chat(sock.sockfd);

    close(sock.sockfd);

    return EXIT_SUCCESS;
}

void *chat(int sockfd) {
    char buff[MAX];
    int n;
    while(1) {
        memset(buff, '\0', MAX);
        n = 0;
        while ((buff[n++] = getchar()) != '\n') {}
        write(sockfd, buff, sizeof(buff));
        if (strncmp("/exit", buff, 5) == 0) {
            return NULL;
        }

        if (strncmp("/shutdown", buff, 9) == 0) {
            return NULL;
        }

    }

}