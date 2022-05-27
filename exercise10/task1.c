#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include "allocator_tests.h"

#define BLOCK_SIZE 1024

void* my_malloc(size_t size);
void my_free(void* ptr);
void my_allocator_init(size_t size);
void my_allocator_destroy(void);

typedef struct Node {
    char block[BLOCK_SIZE];
    struct Node* next;
} node_t;


node_t *head;
node_t *current;

node_t *begin;

int main(void) {
    test_free_list_allocator();
    //my_allocator_init(10000);
    return EXIT_SUCCESS;
}

void *my_malloc(size_t size) {
    if (size >= BLOCK_SIZE) {
        return NULL;
    }

   if (current != NULL) { 
       node_t * my_malloc_address = current;
       current = current->next;
        return my_malloc_address;
   }
   return NULL;
}

void my_free(void* ptr) {
    current = ptr;

}

void my_allocator_init(size_t size) {
    begin = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);
    
    head = begin;
    current = begin;
    head->next = current;

    size_t i = 0;
    /* while(1) {
        i += BLOCK_SIZE;
        if (i > size) {
            break;
        }
        node_t next_node; 
        
        current->next = &next_node;
        current = &next_node;
    } */
    
    while(1) {
        i += sizeof(node_t);
        if (i > size) {
            break;
        }
        node_t *next_node = (node_t *) begin++;
        current->next = next_node;
        current = next_node;

    } 

    current->next = NULL;
    current = head;

}

void my_allocator_destroy(void) {
    if (munmap(begin, BLOCK_SIZE) != 0) {
        perror("munmap\n");
        exit(EXIT_FAILURE);
    } 
}