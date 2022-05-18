#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


struct message {
    char data[100];
}; 
typedef struct message message;

volatile __sig_atomic_t sigflag = 0; //async-signal-safe according to SIG31-C

void handler(int signum) {
    sigflag = signum;
}

int main(int argc, char* argv[]) {

    char* printer_name;
    
    
    struct sigaction sa = {
        .sa_handler = handler,
    };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    

    if (argc != 2) {
        perror("Should look like ./print_server <name of mq>\n");
        return EXIT_FAILURE;
    }
    printer_name = argv[1];

    const int oflag = O_CREAT | O_EXCL;
    const mode_t permissions = S_IRUSR | S_IWUSR;
    const struct mq_attr attr = {.mq_maxmsg = 10, .mq_msgsize = sizeof(message)};
    const mqd_t mq = mq_open(printer_name, oflag, permissions, &attr);
    if (mq == -1) {
        return EXIT_FAILURE; 
    } 

    bool quit_server = false;
    while (!quit_server) {
        
        message msg = {{0}}; 
        unsigned int priority = 0;

        if(mq_receive(mq, (char*)&msg, sizeof(msg), &priority) == -1) {
            switch(sigflag) {
                case SIGINT: quit_server = true; break;
                default: break;
            }
           
        }

        if(!quit_server) {
           printf("Serving print job with priority %d:\n", priority); 
           for (int i = 0; i < (int) strlen(msg.data); i++) {
               switch(sigflag) {
                   case SIGINT: quit_server = true; break;
                   default: 
                        printf("%c", msg.data[i]);
                        fflush(stdout);
                        usleep(200 * 10000);
                        break;
               }
               
           }
        } 
    }

    printf("\nPrint Server shutting down\n");
    mq_close(mq);
    mq_unlink(printer_name);
    return EXIT_SUCCESS;



}