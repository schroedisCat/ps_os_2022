#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#define BLOCK_SIZE 1024

void* my_malloc(size_t size);
void my_free(void* ptr);
void my_allocator_init(size_t size);
void my_allocator_destroy(void);

typedef struct Node {
    char *block;
    struct Node* next;
} node_t;


node_t *head;

int main(void) {
    my_allocator_init(10);
    printf("%p\n", my_malloc(BLOCK_SIZE));

    return EXIT_SUCCESS;
}

void * my_malloc(size_t size) {
    node_t *current_node = head;
    while(current_node != NULL) {
        if (strcmp(current_node->block, "free") == 0) {
            current_node->block = "allocd";
           
            return &current_node;
        } else {
            current_node = current_node->next;
        }
    }
}

void my_free(void* ptr) {
}

void my_allocator_init(size_t size) {

    mmap(NULL, size * BLOCK_SIZE, PROT_NONE, MAP_PRIVATE, 0 ,0);
   
    node_t node_arr[size];
    for (size_t i = 0; i < size; i++) {
        node_arr[i].block = "free";
        if ((i + 1) == size) {
            node_arr[i].next = NULL;
        }
        node_arr[i].next = &node_arr[i + 1];
    } 
}

void my_allocator_destroy(void) {
    //munmap();
}