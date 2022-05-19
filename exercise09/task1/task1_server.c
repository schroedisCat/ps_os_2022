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

void *listener_func(void *ptr);
void *chat_func(void *ptr);

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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock.sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("bind\n");
        exit(EXIT_FAILURE);
    }

    if(listen(sock.sockfd, 5) != 0) {
        printf("listen\n");
        exit(EXIT_FAILURE);
    } else {
        printf ("Server listening on port %d\n", port);
    }

    pthread_t listener_thread;

    pthread_create(&listener_thread, NULL, listener_func, &sock);
    pthread_join(listener_thread, NULL);

    printf("Server is shutting down. ");
    printf("Waiting for %d clients to diconnect.\n", (sock.amount_threads - 1));

    for(int i = 0; i < sock.amount_threads; i++) {
        pthread_join(sock.client_threads[i], NULL);
    }

    if (close(sock.sockfd) != 0) {
        perror("close\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}


void *listener_func(void *ptr) {
    socket_t *sock = (socket_t *)ptr; 
    sock->amount_threads = 0;

    connection_t conn[AMOUNT_THREADS];
    int i = 0;
    while(1) {
       
        conn[i].listener_thread = pthread_self();
        
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        conn[i].connfd = accept(sock->sockfd, (struct sockaddr *)&cli, &len);
        if (conn[i].connfd < 0) {
            printf("server accept failed.\n");
            return NULL;
        } else {
            printf("server accept the client.\n");
            pthread_create(&sock->client_threads[i], NULL, chat_func, &conn[i]);
            sock->amount_threads++;

        }
        
        i++;
    }

    return NULL;
}

void *chat_func(void *ptr) {
    connection_t *conn = (connection_t *) ptr;

    char buff[MAX];

    while(1) {
        //sets all chars to \0
        memset(buff, '\0', MAX);

        if (read(conn->connfd, buff, sizeof(buff)) == -1) {
            perror("read\n");
            exit(EXIT_FAILURE);
        }
            
        //Closes the server
        if(strstr(buff, "/shutdown") != NULL) {
            printf("User disconnected.\n");
            pthread_cancel(conn->listener_thread);
            close(conn->connfd);
            pthread_exit(NULL);
            break;
        }
        //Closes the Connection w/o exiting the server
        // first strncmp is for strg + ] --> quit in telnet 
        // second strncmp is a custom exit
        if (strstr(buff, "/quit") != NULL) {
            printf("User disconnected.\n");
            close(conn->connfd);
            break;
        }
        printf("%s\n", buff);

    }
    return NULL;
}