#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int N = 0; 
    int B = 0;
    char * shm_name = "/task1_csat8697";
    if (argc != 3) {
        perror("Not enough arguments!\n");
        return EXIT_FAILURE;
    } else {
        N = strtol(argv[1], NULL, 10);
        B = strtol(argv[2], NULL, 10);
    }

    int fd = shm_open(shm_name, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); //User has read and write permission
    
    if (fd == -1) {
        perror("opening shared memory\n");
        return EXIT_FAILURE;
    }

    
    if (ftruncate(fd, (B+1) * sizeof(uint64_t)) == -1) {
        perror("truncate not working!\n");
        return EXIT_FAILURE;
    }

    uint64_t *ui64ptr = mmap(NULL, (B+1)*sizeof(uint64_t), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    if (close(fd) == -1) {
        perror("close\n"); 
        return EXIT_FAILURE;
    }

    if (fork() == 0) {

        if (fd == -1) {
            perror("opening shared memory\n");
            return EXIT_FAILURE;
        }        

        for (int i=0; i<N; i++) {
            ui64ptr[i % B] = (i + 1);
        }

        if (munmap(ui64ptr, (B+1)*sizeof(uint64_t)) == -1) {
                perror("unmap\n");
                return EXIT_FAILURE;
        }

        exit(EXIT_SUCCESS);
        
    } else {
        if (fork() == 0) {
            
            if (fd == -1) {
                perror("opening shared memory\n");
                return EXIT_FAILURE;
            }     
            
            uint64_t sum = 0;

            for (int i=0; i<B; i++) {
                sum += ui64ptr[i];
            }
            ui64ptr[B] = sum;
            
            if (munmap(ui64ptr, (B+1)*sizeof(uint64_t)) == -1) {
                perror("unmap\n");
                return EXIT_FAILURE;
            }
            
            exit(EXIT_SUCCESS);

        } else {
            wait(NULL);
            wait(NULL);

            fd = shm_open(shm_name, O_RDWR,  S_IRUSR);

            if (fd == -1) {
                perror("opening shared memory\n");
                return EXIT_FAILURE;
            }    

            printf("Sum of shared memory: %lu\n", ui64ptr[B]);

            if (munmap(ui64ptr, (B+1)*sizeof(uint64_t)) == -1) {
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

    /*
    When running this program with higher number (e.g. 10.000 to 100.000) you get another result every time. That may be cause because of racing condition.
    */

    
}