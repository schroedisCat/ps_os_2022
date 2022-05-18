#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include "myqueue.h"

#define THREADS_AMOUNT 500
#define ENTRIES 100000


myqueue queue;
pthread_mutex_t mutex;
pthread_cond_t more_cond;

typedef struct building_sum {
    int consumer_num;
    int sum;
} building_sum;

void* read_and_sum(void* struct_ptr);


int main (void) {
    
    myqueue_init(&queue);
    

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex inint failed\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_init(&more_cond, NULL) != 0) {
        printf("pthread_cond_init");
        exit(EXIT_FAILURE);
    }


    pthread_t ptid[THREADS_AMOUNT];
    building_sum sum[THREADS_AMOUNT];
    
    for (int i = 0; i < THREADS_AMOUNT; i++) {
        sum[i].sum = 0;
        sum[i].consumer_num = i;
        pthread_create(&ptid[i], NULL, read_and_sum , &sum[i]);
    }

    for (int i = 0; i < ENTRIES; i++) {
        pthread_mutex_lock(&mutex);
        myqueue_push(&queue, 1);
        pthread_cond_signal(&more_cond);
        pthread_mutex_unlock(&mutex);
    }
    
    for (int i = 0; i < THREADS_AMOUNT; i++) {
        myqueue_push(&queue, 0);
    }
    
    for(int i = 0; i < THREADS_AMOUNT; i++) {
        pthread_join(ptid[i], NULL);
    }

    int final_sum = 0;
    for(int i = 0; i < THREADS_AMOUNT; i++) {
        final_sum += sum[i].sum;
    }
    printf("Final sum: %d\n", final_sum);
    
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}

void* read_and_sum(void* struct_ptr) {
    building_sum* sum = (building_sum*) struct_ptr;

    bool exit = false;
    int value;
    
    do {

        pthread_mutex_lock(&mutex);
        while (myqueue_is_empty(&queue)) {
            pthread_cond_wait(&more_cond, &mutex);
        }
        value = myqueue_pop(&queue);
            if (value == 0) {
                exit = true;
            } else {
                sum->sum++;
            }        
        pthread_cond_signal(&more_cond);
        pthread_mutex_unlock(&mutex);

    } while(exit != true);

    printf("Consumer %3d sum: %d\n", sum->consumer_num, sum->sum);
    return NULL;
}
    