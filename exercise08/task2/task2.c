#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>

#define SA struct sockaddr
#define MAX 20

//parts taken from https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("No enough arguments\n");
        exit(EXIT_FAILURE);
    }
    int port = strtol(argv[1], NULL, 10);



    int sockdf = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);


    if(bind(sockdf, (SA*)&addr, sizeof(addr)) != 0) {
        perror("bind\n");
        exit(EXIT_FAILURE);
    }

    if(listen(sockdf, 5) != 0) {
        printf("listen\n");
        exit(EXIT_FAILURE);
    } else {
        printf ("Server listening on port %d\n", port);
    }


    while(1) {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        int connfd = accept(sockdf, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed.\n");
            exit(EXIT_FAILURE);
        }
        else {
            printf("server accept the client.\n");
        }

        char buff[MAX];
        while(1) {
            //sets all chars to \0
            memset(buff, '\0', MAX);

            if (read(connfd, buff, sizeof(buff)) == -1) {
                perror("read\n");
                exit(EXIT_FAILURE);
            }
            printf("Echo: %s\n", buff);
            
            //Closes the server
            if(strncmp("/shutdown", buff, 9) == 0) {
                printf("Shutting down.\n");
                close(sockdf);  
                return EXIT_SUCCESS;
            }
            //Closes the Connection w/o exiting the server
            // first strncmp is for strg + ] --> quit in telnet 
            // second strncmp is a custom exit
            if (strncmp("", buff, 1) == 0 || strncmp("/exit", buff, 5) == 0) {
                printf("Closing connection to client\n");
                printf("Server ready for the next client\n");
                close(connfd);
                break;
            }

        }
    }
       
}