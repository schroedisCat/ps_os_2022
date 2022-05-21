#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdbool.h>
#include <stdlib.h>

#define AMOUNT_THREADS 4
#define AMOUNT_CLIENTS 10

typedef struct socket {
    int sockfd;
    pthread_t client_threads[AMOUNT_THREADS];
} socket_t;

typedef struct socket_client {
    int sockfd;
    char *username;
    pthread_t write_thread;
    pthread_t listen_thread;
} socket_client_t;


typedef struct connection {
    int connfd;
    pthread_t listener_thread;
} connection_t;

typedef struct usernames {
    int connfd;
    char username[20];
} usernames_t;

#endif