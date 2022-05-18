#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>

struct message {
    char data[100];
}; 
typedef struct message message;

int main (int argc, char* argv[]) {
    char* printer_name; 
    int priority = 0;
    FILE* file_name;

    if (argc != 3) {
       perror("Should look like ./print </csXXXX> <priority> < <fileX>\n");
       return EXIT_FAILURE; 
    }

    printer_name = argv[1];
    priority = strtol(argv[2], NULL, 10);
    file_name = stdin;

    const mqd_t mq = mq_open(printer_name, O_WRONLY, 0, NULL);

    char text[32];
    fgets(text, 32, file_name);
    fclose(file_name);
    

    message msg = {{0}}; 
    sprintf(msg.data, "%s\n", text); 

    if (mq_send(mq, (const char*)&msg, sizeof(msg), priority)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}