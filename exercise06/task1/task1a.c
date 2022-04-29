#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int my_global = 1;

void* increment_my_global();

int main(void) {

	printf("In main process: %d\n", my_global);

	pid_t cpid = fork();
	if(cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if(cpid == 0) {
		my_global++;
		exit(EXIT_SUCCESS);
	} else {
		wait(NULL);
		printf("After fork():\t %d\n", my_global); // global variables are not shared after fork()
	}

	pthread_t pthread;

	if(pthread_create(&pthread, NULL, &increment_my_global, NULL) != 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	if(pthread_join(pthread, NULL) != 0) { // waits for the thread (here pthread) to terminate
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	printf("After pthread termination: %d\n", my_global);
	return EXIT_SUCCESS;
}

void* increment_my_global() {
	my_global++;
	return NULL;
}