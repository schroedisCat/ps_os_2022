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
void rearrange_globals(int connfd);
void setUsername(char username[20], int connfd);


usernames_t usernames[AMOUNT_CLIENTS];
int num_threads = 0;
int current_clients = 0;


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
    printf("Waiting for %d clients to diconnect.\n", current_clients);

    for(int i = 0; i < num_threads; i++) {
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
            printf("server accept: ");
            usernames[i].connfd = conn[i].connfd;
            num_threads++;
            current_clients++;
            pthread_create(&sock->client_threads[i], NULL, chat_func, &conn[i]);
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

        printf("%s\n", buff);
        
        //checks if new user is connected to store its username to the right file descriptor
        if(strstr(buff, "connected") != NULL) {
            char *messageString = buff;
            char *newUsername = strtok_r(messageString, " ", &messageString);
            setUsername(newUsername, conn->connfd);
        }
            
        //Closes the server
        if(strstr(buff, "/shutdown") != NULL) {
            printf("User disconnected.\n");
            rearrange_globals(conn->connfd);
            close(conn->connfd);
            pthread_cancel(conn->listener_thread);
            pthread_exit(NULL);
            break;
        }
        //Closes the Connection w/o exiting the server
        if (strstr(buff, "/quit") != NULL) {
            printf("User disconnected.\n");
            rearrange_globals(conn->connfd);
            close(conn->connfd);
            break;
        }
        
        //checks if someone wants to whisper
        if (strstr(buff, "/w ") != NULL) {
            char* rest = buff;
            char whisperMessage[MAX] = "";
            char test[MAX] = "";
            
            strcat(test, strtok_r(rest, " ", &rest));
            strcat(whisperMessage, test);
            strcat(whisperMessage, "(whispers): ");
            strtok_r(rest, " ", &rest);
            char *userToWhisper = strtok_r(rest, " ", &rest);
            
            strcat(whisperMessage, rest);
            
            for (int i = 0; i < num_threads; i++) {
                if (strcmp(userToWhisper, usernames[i].username) == 0) {
                    write(usernames[i].connfd, whisperMessage, sizeof(buff));
                }
            }
        } else {
            for (int i = 0; i < num_threads; i++) {
                if (usernames[i].connfd != -1 && usernames[i].connfd != conn->connfd) {
                    write(usernames[i].connfd, buff, sizeof(buff));
                }
            }
        }
        
        
        
        

    }
    return NULL;
}

void setUsername(char username[20], int connfd) {
    for (int i = 0; i < num_threads; i++) {

        if (usernames[i].connfd == connfd) {
            strcpy(usernames[i].username, username);
            break;
        }
    }

}

void rearrange_globals(int connfd) {
    for (int i = 0; i < num_threads; i++) {
        if (usernames[i].connfd == connfd) {
            usernames[i].connfd = -1;
            strcpy(usernames[i].username, "");
        }
    }
    current_clients--;
    
}