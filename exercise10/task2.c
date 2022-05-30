#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "allocator_tests.h"
#include "membench.h"

void* my_malloc(size_t size);
void my_free(void* ptr);
void my_allocator_init(size_t size);
void my_allocator_destroy(void);



typedef struct Node {
    struct Node* next;
    bool allocated;
    size_t size;
    //__uint8_t *block;
} node_t;

node_t *begin;
node_t *first_free;

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
    //ansonsten überprüfe ALLE speicherbereiche auf den best möglichsten speicherplatz
    //setze den bereich für size an die richtige stelle  
    node_t *next_free_tmp = first_free;
    node_t *best_fit = first_free;
    size_t best_fit_size = first_free->size;
    //node_t *ptr_to_header_before = NULL;

    
    while(next_free_tmp != NULL) {

        //addresses lookin good
        printf("my_malloc: %p where block starts at: %p  and ->next: %p\n", (void *) next_free_tmp, (void *) ((__uint8_t *) next_free_tmp + sizeof(node_t*)),  (void *) next_free_tmp->next);
        if (next_free_tmp->next == NULL) {
            node_t* address_header_tmp = best_fit;

            best_fit->size = size;
            best_fit->allocated = true;
            best_fit->next = (node_t *) ((__uint8_t *) best_fit + sizeof(node_t*) + size);

            //if(ptr_to_header_before != NULL) {
            //    ptr_to_header_before->next = best_fit;
            //}
            
            if (first_free == best_fit) {
                first_free = first_free->next;
            }
                
            //first_free->next = NULL;
            __uint8_t* address_block_tmp = ((__uint8_t *) address_header_tmp) + sizeof(node_t *);
            return address_block_tmp;
        } else {
            if (next_free_tmp->allocated == false) {
                if (next_free_tmp->size < best_fit_size && next_free_tmp->size >= size) {
                    best_fit_size = next_free_tmp->size;
                    best_fit = next_free_tmp;
                }
            }

        }
    
        next_free_tmp = next_free_tmp->next;
    }
    
    return NULL;
}

void my_free(void *ptr) {
    node_t *ptr_header = (node_t *) ((__uint8_t *)ptr - sizeof(node_t *));
        printf("my_free for %p (block_adress) where ->next: %p\n", ptr, (void *) ptr_header->next);

    if (ptr_header < first_free) {
        printf("check this shit\n");
        ptr_header->next = first_free->next;
        printf("ptr_header->next: %p and first_free->next: %p\n", (void*)ptr_header->next, (void*)first_free->next);
        first_free = ptr_header;
        return;
    } else {
        printf("else this fun\n");
        //get not allocated block before ptr
        node_t *free_tmp = first_free;
        node_t *ptr_to_header_before = first_free;

        while(free_tmp != NULL) {
            if (free_tmp == ptr_header) {
                if (ptr_to_header_before != NULL && ptr_to_header_before->allocated == false) {
                    free_tmp->size += ptr_to_header_before->size;
                }
            }
            ptr_to_header_before = free_tmp;
            free_tmp = free_tmp->next;
        }
    }

    if (ptr_header->next != NULL && ptr_header->next->allocated == false) {
        //printf("check if there is a next node\n");
        ptr_header->size += ptr_header->next->size;
        ptr_header->next = (node_t *) ((__uint8_t *)ptr_header->next + ptr_header->next->size);
    }
    

}

void my_allocator_init(size_t size) {
    begin = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    overall_size = size;
    first_free = begin;
    first_free->next = NULL;
    first_free->allocated = false;
}

void my_allocator_destroy(void) {
    if (munmap(begin, overall_size) != 0) {
        perror("munmap\n");
        exit(EXIT_FAILURE);
    }
}