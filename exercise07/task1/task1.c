#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define THREAD_AMOUNT 1000
#define ITERATIONS 10000

int X = 100000;
pthread_mutex_t mutex;
atomic_int atomic_X = 100000;

void* decrementX_Mutex();
void* decrementX_Atomic();

#if USE_MUTEX
#define METHOD decrementX_Mutex
#define DATA X
#define NAME "Mutex"
#else
#define METHOD decrementX_Atomic
#define DATA atomic_X
#define NAME "Atomic"
#endif

int main(void) {
	pthread_t ptid[THREAD_AMOUNT];

	for(int i = 0; i < THREAD_AMOUNT; i++) {
		
		if (pthread_create(&ptid[i], NULL, METHOD, NULL) != 0) {
			perror("pthread_create\n");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < THREAD_AMOUNT; i++) {
		if (pthread_join(ptid[i], NULL) != 0) {
			perror("pthread_join\n");
			exit(EXIT_FAILURE);
		}
	}

	printf("final value of X when using %s: %d\n", NAME, DATA);
	return EXIT_SUCCESS;
}

void* decrementX_Mutex() {
	for(int i = 0; i < ITERATIONS; i++) {
		pthread_mutex_lock(&mutex);
		X--;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* decrementX_Atomic() {
	for(int i = 0; i < ITERATIONS; i++) {
		atomic_X--;
	}
	return NULL;
}