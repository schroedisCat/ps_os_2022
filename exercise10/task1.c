#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

#include "allocator_tests.h"
#include "membench.h"

#define BLOCK_SIZE 1024

void* my_malloc(size_t size);
void my_free(void* ptr);
void my_allocator_init(size_t size);
void my_allocator_destroy(void);

typedef struct Node {
    struct Node* next;
    __uint8_t block[BLOCK_SIZE];
} node_t;


node_t *head;
node_t *next_free;

node_t *end;
node_t *begin;

pthread_mutex_t mutex;

int main(void) {
    test_free_list_allocator();

    run_membench_global(my_allocator_init, my_allocator_destroy, my_malloc, my_free);
    return EXIT_SUCCESS;
}

void *my_malloc(size_t size) {
    if (size > BLOCK_SIZE) {
        return NULL;
    }
    pthread_mutex_lock(&mutex);
   if (next_free != NULL) { 
       __uint8_t *my_malloc_address = next_free->block;
       next_free = next_free->next;
        pthread_mutex_unlock(&mutex);

        return my_malloc_address;
   }
   pthread_mutex_unlock(&mutex);
   return NULL;
}

void my_free(void* ptr) {
    // wenn ptr außerhalb des memory blocks liegt mach nichts
    if ( ptr < (void *) ((__uint8_t *) head - sizeof(node_t)) && ptr > (void *) (end + 1)) {
         return;
    } 

    //ansonsten speichere die adresse vom head des ptrs ab und hänge sie abhängig der Adresse an die richtige stelle der free->liste
    else {
        node_t * ptr_block = (node_t *) ((__uint8_t *)ptr - sizeof(node_t *));

        //falls der zu löschende pointer vor dem next_free ist dann wird es zum next_free
        pthread_mutex_lock(&mutex);
        if (ptr_block < next_free) {
            ptr_block->next = next_free->next;
            next_free = ptr_block;
            pthread_mutex_unlock(&mutex);
            return;
        }
        //ansonsten suche die richtige position in der Liste und hänge um
        node_t *next_free_tmp = next_free;
        while (next_free_tmp != NULL) {
            if (next_free_tmp->next > ptr_block) {
                node_t* tmp_ptr = next_free->next;
                next_free->next = ptr_block;
                ptr_block->next = tmp_ptr;
                break;
            } 
            if (next_free_tmp->next == NULL) {
                ptr_block->next = NULL;
                next_free->next = ptr_block;
                break;
            }
            next_free_tmp = next_free_tmp->next;
        }
        pthread_mutex_unlock(&mutex);
        
    }
}

void my_allocator_init(size_t size) {
    begin = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);
    
    head = begin;
    next_free = begin;
    head->next = begin++;

    size_t i = sizeof(node_t);
    
    while(1) {
        i += sizeof(node_t);
        if (i > size) {
            break;
        }
        node_t *next_node = (node_t *) begin++;
        next_free->next = next_node;
        next_free = next_free->next;

    } 

    next_free->next = NULL;
    end = next_free++;
    next_free = head;

}

void my_allocator_destroy(void) {
    if (munmap(head, BLOCK_SIZE) != 0) {
        perror("munmap\n");
        exit(EXIT_FAILURE);
    } 
}