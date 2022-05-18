#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "myqueue.h"

#define SA struct sockaddr
#define MAX 20
#define THREAD_AMOUNT 4

int sockdf;

typedef struct thread_pool {
    pthread_t ptid[THREAD_AMOUNT];
    myqueue queue;
} thread_pool_t;

thread_pool_t pool;

void *accept_clients();

void pool_create(thread_pool_t* pool, size_t size);
job_id pool_submit(thread_pool_t* pool, job_function start_routine, job_arg arg);
void pool_await(job_id id);
void pool_destroy(thread_pool_t* pool);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("No enough arguments\n");
        exit(EXIT_FAILURE);
    }

    int port = strtol(argv[1], NULL, 10);

    sockdf = socket(AF_INET, SOCK_STREAM, 0);

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

    pthread_t thread_listener;

    //pool_create(&pool, THREAD_AMOUNT);
    pthread_create(&thread_listener, NULL, &accept_clients, NULL);
    
    pthread_join(thread_listener, NULL);
    return EXIT_SUCCESS;
}

void* accept_clients() {

    int connfd[MAX];

    for (int i = 0; i < MAX; i++) {

        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);

        connfd[i] = accept(sockdf, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("server accept failed.\n");
            exit(EXIT_FAILURE);
        } else {
            printf("server accept the client.\n");
        }

        

        while(1) {
        }
    
    }


    return NULL;
}

void pool_create(thread_pool_t* pool, size_t size) {
    myqueue_init(&pool->queue);
    for (size_t i = 0; i < size; i++) {
        pthread_create(&pool->ptid[i], NULL, NULL , &pool);
        
    }
    
}

job_id pool_submit(thread_pool_t* pool, job_function start_routine, job_arg arg) {
    job_id id = myqueue_push(&pool->queue, start_routine, arg);
    return id;
}

void pool_await(job_id id) {
    sem_wait(&id);
}

void pool_destroy(thread_pool_t* pool) {
    //loop to remove all remaining data from the queue
    for (size_t i = 0; i < THREAD_AMOUNT; i++) {
        pthread_join(pool->ptid[i], NULL);
    }
}
