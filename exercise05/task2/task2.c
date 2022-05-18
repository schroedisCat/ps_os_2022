#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
    int N = 0; 
    int B = 0;
    
    char * shm_name = "/task2_csat8697";
    if (argc != 3) {
        perror("Not enough arguments!\n");
        return EXIT_FAILURE;
    } else {
        N = strtol(argv[1], NULL, 10);
        B = strtol(argv[2], NULL, 10);
    }

   

    int fd = shm_open(shm_name, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); //User have read and write permission

    if (fd == -1) {
        perror("opening shared memory\n");
        return EXIT_FAILURE;
    }

    
    if (ftruncate(fd, (B+1)*sizeof(uint64_t)) == -1) {
        perror("truncate not working!\n");
    }

    uint64_t *buffer = mmap(NULL, (B+1)*sizeof(uint64_t) + 2*sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_t *semptr = (sem_t*) &buffer[B+1];

    if (sem_init(semptr, 1, B) != 0) {  //'empty'- counter starting with the available places in the array
        perror("Semaphore init empty!\n");
        return EXIT_FAILURE;
    }

    if (sem_init(semptr + 1, 1, 0) != 0) { //'full' - counter starting by 0 (because no data is stored at the beginning)
        perror("Semaphore init full!\n");
        return EXIT_FAILURE;
    }
    
    if (fork() == 0) {

        for (int i=0; i<N; i++) {
            sem_wait(semptr);  // wait if buffer is full ('empty'-semaphore by 0)
            buffer[i % B] = (i + 1); 
            sem_post(semptr + 1);  // increments 'full'-semaphore by 1
        }

         if (munmap(buffer, (B+1)*sizeof(uint64_t) + 2*sizeof(sem_t)) == -1) {
                perror("unmap\n");
                return EXIT_FAILURE;
        }
        
        exit(EXIT_SUCCESS);
        
    } else {
        if (fork() == 0) {
            uint64_t sum = 0;
            for (int i=0; i<N; i++) {
                sem_wait(semptr + 1); // wait if buffer is empty ('full'-semaphore by 0)
                sum += buffer[i % B];           
                sem_post(semptr); // increments 'empty'-semaphore by 1                 
            }
            buffer[B] = sum;

            if (munmap(buffer, (B+1)*sizeof(uint64_t) + 2*sizeof(sem_t)) == -1) {
                perror("unmap\n");
                return EXIT_FAILURE;
            }

            exit(EXIT_SUCCESS);

        } else {
            wait(NULL);
            wait(NULL);

            
            printf("Sum of shared memory: %lu\n", buffer[B]);

            if(sem_destroy(semptr) == -1) {
                perror("sem_destroy!\n");
                return EXIT_FAILURE;
            }

            if(sem_destroy(semptr + 1) == -1) {
                perror("sem_destroy!\n");
                return EXIT_FAILURE;
            }

            if (munmap(buffer, (B+1)*sizeof(uint64_t) + 2*sizeof(sem_t)) == -1) {
                perror("unmap\n");
                return EXIT_FAILURE;
            }

            if (close(fd) == -1) {
                perror("close\n"); 
                return EXIT_FAILURE;
            }

            if (shm_unlink(shm_name) == -1) {
                perror("unlink\n");
                return EXIT_FAILURE;
            }
            
            
            return EXIT_SUCCESS;
        }
    }
}

