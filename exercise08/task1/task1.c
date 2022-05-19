#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#include "myqueue.h"

#define COUNTER 2000
#define THREAD_AMOUNT_A 2000
#define THREAD_AMOUNT_B 4

#if USE_THREAD_POOL
#define TASK "task1a"
#else
#define TASK "task1b"
#endif

typedef struct counter {
    atomic_int *counter_ptr;
} counter_t;

typedef struct thread_pool {
    pthread_t ptid[THREAD_AMOUNT_B];
    myqueue queue;
} thread_pool_t;

thread_pool_t pool;

sem_t semaphore;
pthread_mutex_t mutex;

void * decrement_counter(void * ptr); 
void * getQueueFunction();

void pool_create(thread_pool_t* pool, size_t size);
job_id pool_submit(thread_pool_t* pool, job_function start_routine, job_arg arg);
void pool_await(job_id id);
void pool_destroy(thread_pool_t* pool);


int main(void) {


    //init counter struct
    atomic_int counter = COUNTER;
    counter_t cnt_struct; 
    cnt_struct.counter_ptr = &counter;

    if (strcmp(TASK, "task1a")) {

        pthread_t ptid[THREAD_AMOUNT_A];

        printf("In task 1a with %d threads\n", THREAD_AMOUNT_A);
        for (int i = 0; i < THREAD_AMOUNT_A; i++) {
            if (pthread_create(&ptid[i], NULL, &decrement_counter, &cnt_struct) != 0) {
                perror("pthread_create\n");
                exit(EXIT_FAILURE);
            }
        }

        for(int i = 0; i < THREAD_AMOUNT_A; i++) {
            if (pthread_join(ptid[i], NULL) != 0) {
                perror("pthread_join\n");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        printf("In task 1b with %d threads\n", THREAD_AMOUNT_B);
        job_function func = &decrement_counter; 
        job_id id[COUNTER];

        sem_init(&semaphore, 1, 0);
        pthread_mutex_init(&mutex, NULL);

        pool_create(&pool, THREAD_AMOUNT_B);

        //working loop
        for (int i = 0; i < COUNTER; i++) {
            id[i] = pool_submit(&pool, func, &cnt_struct);
        }

        //terminating loop
        for (int i = 0; i < THREAD_AMOUNT_B; i++) {
            id[i] = pool_submit(&pool, NULL, NULL);
        }

        for (int i = 0; i < COUNTER; i++) {
            pool_await(id[i]);
        }

        pool_destroy(&pool);
        pthread_mutex_destroy(&mutex);
        sem_destroy(&semaphore);
    }

    printf("final value is: %d\n", counter);
    return EXIT_SUCCESS;
}


void pool_create(thread_pool_t* pool, size_t size) {
    myqueue_init(&pool->queue);
    for (size_t i = 0; i < size; i++) {
        pthread_create(&pool->ptid[i], NULL, &getQueueFunction , &pool);
        
    }
    
}

job_id pool_submit(thread_pool_t* pool, job_function start_routine, job_arg arg) {
    pthread_mutex_lock(&mutex);
    job_id id = myqueue_push(&pool->queue, start_routine, arg);
    sem_post(&semaphore);
    pthread_mutex_unlock(&mutex);
    return id;
}

void pool_await(job_id id) {
    sem_wait(&id);
}

void pool_destroy(thread_pool_t* pool) {
    //loop to remove all remaining data from the queue
    for (size_t i = 0; i < THREAD_AMOUNT_B; i++) {
        pthread_join(pool->ptid[i], NULL);
    }
}


void* decrement_counter(void* ptr) {
    counter_t* cnt = (counter_t *) ptr;
    --(*cnt->counter_ptr); //muss zuerst dedifferenziert werden damit man nicht auf den pointer sondern auf den Wert zugreifen kann. 
    return NULL; 
}

void* getQueueFunction() {
    job_t job = {0};
    while(1) {
        sem_wait(&semaphore);
        pthread_mutex_lock(&mutex);
        if (! myqueue_is_empty(&pool.queue)) {
            job = myqueue_pop(&pool.queue);           
        }
        pthread_mutex_unlock(&mutex);
        if (job.func == NULL) {
                return NULL;
        } 

        job.func(job.arg);
        
    }
}