#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

#include "allocator_tests.h"
#include "membench.h"

void* my_malloc(size_t size);
void my_free(void* ptr);
void my_allocator_init(size_t size);
void my_allocator_destroy(void);



typedef struct Node {
    struct Node* before;
    struct Node* next;
    size_t size;
    //__uint8_t *block;
} node_t;

node_t *begin;
node_t *next_free;

size_t overall_size;

int main(void){

    test_best_fit_allocator();
    return EXIT_SUCCESS;
}

void *my_malloc(size_t size) {
    //wenn size zu groß ist return null;
    if (size > overall_size) {
        return NULL;
    }
    //wenn die noch nichts allociert ist dann split zum ersten mal
    if (next_free == begin) {
        printf("in next_free == begin\n");
        next_free->size = size;
        next_free->next = (node_t *) ((__uint8_t *) next_free + sizeof(node_t*) + size);
        
        next_free = next_free->next;
        next_free->before = begin;
        next_free->next = NULL;
        //printf("begin: %p\n", (void*) begin);
        //printf("next_free->before: %p\n", (void *) next_free->before);
        //printf("next_free: %p\n", (void *)next_free);
        //printf("start of storage: %p\n", (void *) begin);
        //return ((__uint8_t *) next_free->before + sizeof(node_t));
        return next_free->before;
    }
    //ansonsten überprüfe ALLE freien speicherbereiche
    //setze den bereich für size an die richtige stelle 
    //und schmeiß den block aus dem free raus 
    node_t *next_free_tmp = next_free;
    node_t *best_fit;
    while(next_free_tmp != NULL) {
        if (next_free_tmp->next == NULL) {
            printf("in next_free_tmp->next == NULL\n");
            printf("%ld\n", sizeof(node_t *));
            next_free_tmp->next =  (node_t *) ((__uint8_t *) next_free + sizeof(node_t*) + size);
            node_t *before_next_free_tmp = next_free_tmp->before;
            next_free->size = size;
            next_free = next_free_tmp;
            next_free->before = before_next_free_tmp;
            next_free->next = NULL;

            return next_free;
        } else {

        }
        next_free_tmp = next_free_tmp->next;
    }
    
}

void my_free(void *ptr) {
    printf("my_free for %p\n", ptr);
    node_t *ptr_node = (node_t *) ((__uint8_t *)ptr - sizeof(node_t *));
    printf("start of storage at %p\n", (void *) begin);
    printf("header is at %p\n", (void *)ptr_node);
    if (ptr_node->before != NULL && ptr_node->before->size != 0) {
        printf("checkt if there is a node before\n");
        ptr_node->size += ptr_node->before->size;
        ptr_node = ptr_node->before;
        
    }
    if (ptr_node->next != NULL &&  ptr_node->next->size != 0) {
        printf("check if there is a next node\n");
        ptr_node->size += ptr_node->next->size;
        ptr_node->next = (node_t *) ((__uint8_t *)ptr_node->next + ptr_node->next->size);
    }
    

}

void my_allocator_init(size_t size) {
    begin = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    overall_size = size;
    next_free = begin;
}

void my_allocator_destroy(void) {
    if (munmap(begin, overall_size) != 0) {
        perror("munmap\n");
        exit(EXIT_FAILURE);
    }
}