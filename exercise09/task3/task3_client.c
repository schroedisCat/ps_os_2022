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

void *write_chat(void *ptr);
void *listen_chat(void *ptr);
void *send_username(int sockfd, char *username);

int main(int argc, char* argv[]) {

    if (argc != 3) {
        perror("No enough arguments\n");
        exit(EXIT_FAILURE);
    }
    int port = strtol(argv[1], NULL, 10);
    char *username = argv[2];

    socket_client_t sock_client;

    
    sock_client.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sock_client.username = argv[2];

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //difference, only use for client

    if (connect(sock_client.sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("connection with server failed.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("connected to server.\n");
    }

    send_username(sock_client.sockfd, username);


    pthread_create(&sock_client.write_thread, NULL, write_chat, &sock_client);
    pthread_create(&sock_client.listen_thread, NULL, listen_chat, &sock_client);
    
    pthread_join(sock_client.write_thread,NULL);
    pthread_join(sock_client.listen_thread,NULL);

    if (close(sock_client.sockfd) != 0) {
        perror("close\n");
        exit(EXIT_FAILURE);
    };

    return EXIT_SUCCESS;
}

void *write_chat(void * ptr) {
    socket_client_t *sock_client = (socket_client_t *) ptr;

    char buff[MAX];
    int n;
    int len_name = strlen(sock_client->username) + 2;

    while(1) {
        memset(buff, '\0', MAX);
        strcat(buff, sock_client->username);
        strcat(buff, ": ");
        n = len_name;
        while ((buff[n++] = getchar()) != '\n') {}
        write(sock_client->sockfd, buff, sizeof(buff));

        if (strstr(buff, "/quit") != NULL) {
            pthread_cancel(sock_client->listen_thread);
            return NULL;
        }

        if (strstr(buff, "/shutdown") != NULL) {
            pthread_cancel(sock_client->listen_thread);
            return NULL;
        }

    }

}

void *listen_chat(void *ptr) {
    socket_client_t *sock_client = (socket_client_t *) ptr;

    char buff[MAX];

    while(1) {
        memset(buff, '\0', MAX);
         if (read(sock_client->sockfd, buff, sizeof(buff)) == -1) {
            perror("read\n");
            exit(EXIT_FAILURE);
        }
        printf("%s\n", buff);
    }

}

void *send_username(int sockfd, char *username) {
    char buff[MAX] = "";
    strcat(buff, username);
    strcat(buff, " connected\n");
    
    write(sockfd, buff, sizeof(buff));
    
    return NULL;
}