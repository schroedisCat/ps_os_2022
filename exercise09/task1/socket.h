#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdbool.h>
#include <stdlib.h>

#define AMOUNT_THREADS 4

typedef struct socket {
    int sockfd;
    pthread_t client_threads[AMOUNT_THREADS];
    int amount_threads;
} socket_t;


typedef struct connection {
    int connfd;
    pthread_t listener_thread;
} connection_t;

#endif