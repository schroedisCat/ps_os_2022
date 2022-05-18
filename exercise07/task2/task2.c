#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "myqueue.h"
#include <string.h>

#define ENTRIES 1000000

#if USE_MY_MUTEX
#define MUTEX_LOCK my_mutex_lock()
#define MUTEX_UNLOCK my_mutex_unlock()
#define METHOD "Atomic"
#else 
#define MUTEX_LOCK normal_mutex_lock()
#define MUTEX_UNLOCK normal_mutex_unlock()
#define METHOD "Mutex"
#endif

myqueue queue;
pthread_mutex_t mutex;
atomic_flag lock = ATOMIC_FLAG_INIT;


void* write_queue();
void* read_and_sum();


int main (void) {

    myqueue_init(&queue);

    pthread_t ptid1;
    pthread_t ptid2;


    if (strcmp(METHOD, "Mutex")) {
        if(pthread_mutex_init(&mutex, NULL) != 0) {
            printf("Mutex init failed\n");
            exit(EXIT_FAILURE);
        }
    }
    

    if (pthread_create(&ptid1, NULL, &write_queue, NULL) != 0) {
        printf("First ptrhead_create\n");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_create(&ptid2, NULL, &read_and_sum, NULL) != 0) {
        printf("Second ptrhead_create\n");
        exit(EXIT_FAILURE);       
    }

    if (pthread_join(ptid1, NULL) != 0) {
        printf("First pthread_join\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(ptid2, NULL) != 0) {
        printf("Second pthread_join\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(METHOD, "Mutex")) {
        if (pthread_mutex_destroy(&mutex) != 0) {
            perror("pthread_mutex_destroy\n");
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}


bool my_mutex_lock(){
    while(1) {
        if (! atomic_flag_test_and_set(&lock)) {
            return true;
        }
    }
    
}

void my_mutex_unlock() {
    atomic_flag_clear(&lock);
}

bool normal_mutex_lock() {
    if(pthread_mutex_lock(&mutex) != 0) {
        perror("pthread_mutex_lock\n");
        exit(EXIT_FAILURE);
    }
    return true;
}

void normal_mutex_unlock() {
    if(pthread_mutex_unlock(&mutex) != 0) {
        perror("pthread_mutex_unlock\n");
        exit(EXIT_FAILURE);
    }
    
}

//false == 0 --> alles andere == true
//thread_mutex_lock returned 0 wenn alles passt
void* write_queue() {
    int entries = ENTRIES;
    while(1) {
        if (MUTEX_LOCK) {
            myqueue_push(&queue, 1);
            entries--;
            if (entries == 0) {
                myqueue_push(&queue, 0);
                MUTEX_UNLOCK;
                return NULL;  
            }
            MUTEX_UNLOCK;
        }
    }  

}

void* read_and_sum() {
    int sum = 0;
    int value;
    while(1) {
        if (MUTEX_LOCK) {
            if (! myqueue_is_empty(&queue)) {
                value = myqueue_pop(&queue);

                //exit value;
                if(value == 0) {
                    printf("Sum when using %s: %d\n", METHOD, sum);
                    MUTEX_UNLOCK;
                    return NULL;
                } 
                sum++;
                
            }
           MUTEX_UNLOCK;
        }
    }
}

