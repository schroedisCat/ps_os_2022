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

    close(sock.sockfd);

    return EXIT_SUCCESS;
}


void *listener_func(void *ptr) {
    socket_t *sock = (socket_t *)ptr; 

    bool run_server = true;

    while(run_server) {

        struct sockaddr_in cli; 
        socklen_t len = sizeof(cli);

        connection_t conn;
        pthread_t client_thread;

        while(1) {
            conn.connfd = accept(sock->sockfd, (struct sockaddr *)&cli, &len);
            if (conn.connfd < 0) {
                printf("server accept failed.\n");
                return NULL;
            } else {
                printf("server accept the client.\n");
            

                pthread_create(&client_thread, NULL, chat_func, &conn);
            }
        }   

        
        response_t response;
        printf("before join\n");

        pthread_join(client_thread, (void **)  &response);
        //response_t *response = (response_t *) returnValue;
        printf("after join\n");

        if(response.shutdown) {
            printf("In response\n");
            break;
        }
        
            
        

    }

    return NULL;
}

void *chat_func(void *ptr) {
    connection_t *conn = (connection_t *) ptr;
    response_t cli_response;
    char buff[MAX];

    while(1) {
        //sets all chars to \0
        memset(buff, '\0', MAX);

        if (read(conn->connfd, buff, sizeof(buff)) == -1) {
            perror("read\n");
            exit(EXIT_FAILURE);
        }
        printf("Echo: %s\n", buff);
            
        //Closes the server
        if(strncmp("/shutdown", buff, 9) == 0) {
            printf("Shutting down.\n");
            close(conn->connfd);
            cli_response.shutdown = true;
            break;
        }
        //Closes the Connection w/o exiting the server
        // first strncmp is for strg + ] --> quit in telnet 
        // second strncmp is a custom exit
        if (strncmp("/quit", buff, 5) == 0) {
            printf("Closing connection to client\n");
            printf("Server ready for the next client\n");
            close(conn->connfd);
            cli_response.shutdown = false;
            break;
        }

    }
    printf("returning things\n");
    return NULL;
}