#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


double mc_pi(int64_t S);

int main(int argc, char* argv[]) {
    int N = 0; 
    int S = 0;
    if (argc == 3) {
        N = atoi(argv[1]);
        S = atoi(argv[2]);        
    } else {
        printf("Not enough arguments!\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            srand(getpid());
            printf("Child %3d PID= %d. mc_pi(%d) = %f\n", i , getpid(), S, mc_pi(S));
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < N; i++) {
        wait(NULL);
    }
 
    printf("done.\n");
    return EXIT_SUCCESS;

}


double mc_pi(int64_t S) {
    int64_t in_count = 0;
    for(int64_t i = 0; i < S; ++i) {
        const double x = rand() / (double)RAND_MAX;
        const double y = rand() / (double)RAND_MAX;
        if(x*x + y*y <= 1.f) {
            in_count++;
        }
    }
    return 4 * in_count / (double)S;
}