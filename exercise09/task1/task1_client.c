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

void *chat(int sockfd, char *username);
void *send_username(int sockfd, char *username);

int main(int argc, char* argv[]) {

    if (argc != 3) {
        perror("No enough arguments\n");
        exit(EXIT_FAILURE);
    }
    int port = strtol(argv[1], NULL, 10);
    char *username = argv[2];

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

    send_username(sock.sockfd, username);
    chat(sock.sockfd, username);

    if (close(sock.sockfd) != 0) {
        perror("close\n");
        exit(EXIT_FAILURE);
    };

    return EXIT_SUCCESS;
}

void *chat(int sockfd, char *username) {
    char buff[MAX];
    int n;
    int len_name = strlen(username) + 2;

    while(1) {
        memset(buff, '\0', MAX);
        strcat(buff, username);
        strcat(buff, ": ");
        n = len_name;
        while ((buff[n++] = getchar()) != '\n') {}
        write(sockfd, buff, sizeof(buff));
        if (strstr(buff, "/quit") != NULL) {
            return NULL;
        }

        if (strstr(buff, "/shutdown") != NULL) {
            return NULL;
        }

    }

}

void *send_username(int sockfd, char *username) {
    char buff[MAX] = "";
    strcat(buff, username);
    strcat(buff, " connected\n");
    
    write(sockfd, buff, sizeof(buff));
    
    return NULL;
}