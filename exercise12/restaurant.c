#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "myqueue.h"

//2 global queues for counter and one for ordering
myqueue queue_counter;
myqueue queue_order;

//pthreads mutexes and condition variables, a pair for each queue
pthread_cond_t cond_variable;
pthread_cond_t counter_cond;
pthread_mutex_t mutex_counter;
pthread_mutex_t mutex_order;

//end-marker for cook thread, exits the method)
bool exit_cooks = false;
//boolean for the notification addon of the exam
bool notifications_enabled = false;

//pthread methods for cooks and guests
void *cook_meal(void *ptr);
void *order_and_wait_impatiently(void *ptr);
void *notification_and_chill(void *ptr);

//a guest struct to get information into the pthread method
typedef struct guest_info {
    int guest_nr; 
    int ordering_nr;
    uint64_t waiting_time_ms;
    pthread_cond_t guest_cond_var; //only needed for notifications
} guest_info;

//a guest struct to get information into the pthread method
typedef struct cook_info {
    int cook_nr;
    guest_info *guests_info; //ptr to every guest_info
} cook_info;

int main(int argc, char* argv[]) {
    //checks if there are enough arguments
    if (argc != 4) {
        perror("parameter should look like: ./restaurant <enable notifications> <number of guests> <number of cooks>\n");
        exit(1);
    }

    //strtol to get the numbers of the char array (problem if there is a text)
    notifications_enabled = strtol(argv[1], NULL, 10);
    int number_of_guests = strtol(argv[2], NULL, 10);
    int number_of_cooks = strtol(argv[3], NULL, 10);

    //creating arrays of pthreads. Every cook and guest has its own thread
    pthread_t cooks[number_of_cooks];
    pthread_t guests[number_of_guests];

    //arrays of info. Every cook and guest has its own info
    cook_info cooks_info[number_of_cooks];
    guest_info guests_info[number_of_guests];

    //initializing of the Queues
    myqueue_init(&queue_counter);
    myqueue_init(&queue_order);

    //initializing for the mutexes and cond_variables
    //if the method don't return 0 there is an error
    if(pthread_mutex_init(&mutex_counter, NULL) != 0) {
        printf("Mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_mutex_init(&mutex_order, NULL) != 0) {
        printf("Mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_init(&cond_variable, NULL) != 0) {
        printf("pthread_cond_init\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_init(&counter_cond, NULL) != 0) {
        printf("pthread_cond_init\n");
        exit(EXIT_FAILURE);
    }

    

    // getting the right info stored and creating the pthreads with the guests method
    // if notifications are enabled the condition_var is also initialized and the other method is used
    if (notifications_enabled == true) {

        for (int i = 0; i < number_of_guests; i++) {
            guests_info[i].guest_nr = i;
            guests_info[i].ordering_nr = i;
            if(pthread_cond_init(&guests_info[i].guest_cond_var, NULL) != 0) {
                printf("pthread_cond_init\n");
                exit(EXIT_FAILURE);
            }
            pthread_create(&guests[i], NULL, notification_and_chill, &guests_info[i]);

        }

    } else {
        
        for (int i = 0; i < number_of_guests; i++) {
            guests_info[i].guest_nr = i;
            guests_info[i].ordering_nr = i;
            pthread_create(&guests[i], NULL, order_and_wait_impatiently, &guests_info[i]);
        }

    }

    // getting the right info stored and creating the pthreads with the method cook_meal
    for (int i = 0; i < number_of_cooks; i++) {
        cooks_info[i].cook_nr = i;
        cooks_info[i].guests_info = guests_info;
        pthread_create(&cooks[i], NULL, cook_meal, &cooks_info[i]);
    }


    uint64_t overall_time = 0;
    // loop to wait until all guests pthreads are finished (got their meal) and to sum up the overall_time
    for (int i = 0; i < number_of_guests; i++) {
        pthread_join(guests[i], NULL);
        overall_time += guests_info[i].waiting_time_ms; 

    }

    //setting the exit variable to true to stop all cooks from producuing more meals
    exit_cooks = true;
    
    //sending a signal to get the cook_threads over the cond_var_wait and therefor exiting the thread
    for (int i = 0; i < number_of_cooks; i++) {
        pthread_cond_signal(&cond_variable);
    }

    // loop to wait until all cooks pthreads are finished (all guests got their meal)
    for (int i = 0; i < number_of_cooks; i++) {
        pthread_join(cooks[i], NULL);
    }

    //last message before exiting. The info should be stored in the guest info and the calculate the average 
    //didn't manage to finish it in time. But now it works 
    printf("All guests have been served with an average wait time for %ld ms\n", overall_time / number_of_guests);

    //destroy the mutexes that there are no memory leaks available
    pthread_mutex_destroy(&mutex_counter);
    pthread_mutex_destroy(&mutex_order);
    
    return EXIT_SUCCESS;
}

//method if notifications are enabled for guests
void *notification_and_chill(void *ptr) {
    guest_info* info = (guest_info *) ptr;

    //the order (number) will be stored in the order_queue;
    pthread_mutex_lock(&mutex_order);
    myqueue_push(&queue_order, info->ordering_nr);
    pthread_cond_signal(&cond_variable);
    pthread_mutex_unlock(&mutex_order);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);


    printf("Guest %d has made meal order %d\n",info->guest_nr, info->ordering_nr);
    uint64_t start_ms = ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;

    // the methods now waits until the right number is in the counter_queue and the cond_var gets a signal 
    pthread_cond_wait(&info->guest_cond_var, &mutex_counter); 
    
    myqueue_pop(&queue_counter);

    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t end_ms = ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
    info->waiting_time_ms = end_ms - start_ms;
    printf("Guest %d has picked up order %d after %ld ms\n", info->guest_nr, info->ordering_nr, info->waiting_time_ms);
    pthread_cond_signal(&counter_cond); // sends signal that the counter is free again
    pthread_mutex_unlock(&mutex_counter);


    return NULL;
}

// guests placing their order and wait until it is cooked (in the queue) then exiting
void *order_and_wait_impatiently(void *ptr) {
    guest_info* info = (guest_info *) ptr;

    //the order (number) will be stored in the order_queue;
    pthread_mutex_lock(&mutex_order);
    myqueue_push(&queue_order, info->ordering_nr);
    pthread_cond_signal(&cond_variable);
    pthread_mutex_unlock(&mutex_order);

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);


    printf("Guest %d has made meal order %d\n",info->guest_nr, info->ordering_nr);
    uint64_t start_ms = ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;

    int order_nr;
    //here the method simulates the walk to the counter and the check if the real meal is allreade at the counter
    while(1) {
        usleep(100 * 1000);
        pthread_mutex_lock(&mutex_counter);
        if (! myqueue_is_empty(&queue_counter)) {
            usleep(100 * 1000);
            // the meal need to be popped to check if the order_nr is right
            order_nr = myqueue_pop(&queue_counter);
            if (info->ordering_nr == order_nr) {
                clock_gettime(CLOCK_MONOTONIC, &ts);
                uint64_t end_ms = ts.tv_sec * 1e3 + ts.tv_nsec / 1e6;
                info->waiting_time_ms = end_ms - start_ms;
                printf("Guest %d has picked up order %d after %ld ms\n", info->guest_nr, order_nr, info->waiting_time_ms);
                pthread_cond_signal(&counter_cond);
                pthread_mutex_unlock(&mutex_counter);
                return NULL;
            } else {
                //otherwise it need to get back into the queue
                myqueue_push(&queue_counter, order_nr);
            }
        }
        pthread_mutex_unlock(&mutex_counter);
    }
}

//cooks wait until an order (or more) is in the ordering queue and then cooks the meal. 
//After that they will start to whait for the next order.
void *cook_meal(void *ptr) {
    cook_info* info = (cook_info *) ptr;

    int order_nr;
    while(exit_cooks != true) {

        pthread_mutex_lock(&mutex_order);
        while(myqueue_is_empty(&queue_order)) {
            //waits until the ordering queue is not empy
            pthread_cond_wait(&cond_variable, &mutex_order);
            //close condition
            if (exit_cooks == true) {
                pthread_mutex_unlock(&mutex_order);
                return NULL;
            }
            
        }
        order_nr = myqueue_pop(&queue_order);
        pthread_mutex_unlock(&mutex_order);
        printf("Cook %d is preparing order %d\n", info->cook_nr, order_nr);
        srand(time(NULL)); //to genereate random numbers
        usleep((rand() % 5 + 1) * 100 * 1000); //for a number between 100 and 500 ms
        printf("Cook %d has finished order %d\n", info->cook_nr, order_nr);
        pthread_mutex_lock(&mutex_counter);
        while(! myqueue_is_empty(&queue_counter)) {
            //waits until the counter (-queue) is empty to put the next meal on the table
            pthread_cond_wait(&counter_cond, &mutex_counter);
        }
        myqueue_push(&queue_counter, order_nr);

        // with notifications a signal will be send to the right guest condition variable
        if (notifications_enabled == true) {
            pthread_cond_signal(&info->guests_info[order_nr].guest_cond_var);
        }
        printf("Cook %d has placed order %d on counter\n", info->cook_nr, order_nr);
        pthread_mutex_unlock(&mutex_counter);
        
    }
    return NULL;
}