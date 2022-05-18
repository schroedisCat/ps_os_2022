#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct socket {
    int sockfd;
} socket_t;


typedef struct connection {
    int connfd;
} connection_t;

#endif