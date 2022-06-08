#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> //for write()
#define __USE_GNU // for RTLD_NEXT
#include <dlfcn.h> //for dlopen and dlerror


void print_number(size_t number) {
	if(number > 9) {
		print_number(number / 10);
	}
	const char digit = '0' + number % 10;
	write(STDOUT_FILENO, &digit, 1);
}

void * malloc(size_t size) {
    write(STDOUT_FILENO, "allocating ", 12);
    print_number(size);
    write(STDOUT_FILENO, " bytes\n", 8);
    
    void *(*original_malloc)(size_t);
    
    *(void **) (&original_malloc) = dlsym(RTLD_NEXT, "malloc");
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }
    
    return (*original_malloc)(size);
}

// Funktionsaufruf: LD_PRELOAD=./malloc_spy.so /bin/ls