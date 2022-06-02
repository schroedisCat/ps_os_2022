#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        perror("too less arguments. Command should look like ./map_number <initial_number> <./shared_library1.so> ...\n");
        return(EXIT_FAILURE);
    }
    int init_num = strtol(argv[1], NULL, 10);
    int result = init_num;
    

    void* handler; 
    int (*method)(int);
    char* error;
    
    for (int i = 2; i < argc; i++) {
        handler = dlopen(argv[i], RTLD_LAZY);
        if (!handler) {
            fprintf(stderr, "%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        dlerror();
        *(void **) (&method) = dlsym(handler, "method");
        error = dlerror();
        if (error != NULL) {
            fprintf(stderr, "%s\n", error);
            exit(EXIT_FAILURE);
        }
        result = (*method)(result);
        dlclose(handler);
    }

    printf("%d\n", result); 
    
    return EXIT_SUCCESS;
}