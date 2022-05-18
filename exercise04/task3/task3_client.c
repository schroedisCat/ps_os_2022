#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE_BUF 4096

void writer(const char* name) {
	const int fd = open(name, O_WRONLY);

	if(fd < 0) {
		exit(EXIT_FAILURE);
	}
	char msg[PIPE_BUF];
	while(1) {
		printf("Expression: ");
		fgets(msg, PIPE_BUF, stdin);
		write(fd, msg, strlen(msg) + 1);
	}

	close(fd);
}
int main(int argc, char* argv[]) {
	if(argc != 2) {
		printf("No user named!");
		return EXIT_FAILURE;
	}
	char name[100] = "/tmp/csat8697_pipe_";
	strcat(name, argv[1]);
	writer(name);
	return EXIT_SUCCESS;
}