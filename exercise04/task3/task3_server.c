#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PIPE_BUF 4096

float getExpressionResult(char buf[PIPE_BUF]) {

	char delimiter[] = " ";
	char* ptr;
	char* results[3];

	ptr = strtok(buf, delimiter); // take string apart
	int str_parts = 0;
	while(ptr != NULL) {
		results[str_parts] = ptr;
		str_parts++;
		ptr = strtok(NULL, delimiter);
	}
	if(str_parts != 3) {
		return 0;
	}

	float num1 = strtof(results[0], NULL);
	float num2 = strtof(results[2], NULL);

	if((num1 == 0) | (num2 == 0)) {
		printf("No number\n");
		return 0;
	}

	switch(results[1][0]) {
		case '+': return num1 + num2;
		case '-': return num1 - num2;
		case '*': return num1 * num2;
		case '/': return num1 / num2;
		default: return 0;
	}
}

int main(int argc, char* argv[]) {
	int amount_clients = argc - 1;
	/*
	S_IRUSR --> R for owner
	S_IWUSR --> W for owner
	S_IRGRP --> R for group
	S_IROTH --> R for other
	*/
	const mode_t permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	struct pollfd pfds[amount_clients];
	char name[100] = "/tmp/csat8697_pipe_";

	for(int i = 1; i < argc; i++) {
		char tmpname[100] = "/tmp/csat8697_pipe_";
		strcat(tmpname, argv[i]);

		if(mkfifo(tmpname, permission) != 0) {
			perror("Make Fifo.\n");
			exit(EXIT_FAILURE);
		}

		int fd = open(tmpname, O_RDONLY); // program will stop here until right client has opened
		                                  // the pipe on the other side

		printf("%s connected.\n", argv[i]);

		if(fd < 0) {
			perror("Named pipe not opened correctly.\n");
			exit(EXIT_FAILURE);
		}

		pfds[i - 1].fd = fd;
		pfds[i - 1].events = POLLIN;
	}

	char buf[PIPE_BUF];
	int current_amount_clients = amount_clients;

	while(1) {
		if(poll(pfds, amount_clients, -1) < 0) {
			perror("poll failed");
			exit(EXIT_FAILURE);
		}
		if(current_amount_clients > 0) {
			for(int i = 0; i < amount_clients; i++) {
				switch(pfds[i].revents) {

					case POLLIN:
						read(pfds[i].fd, buf, PIPE_BUF);
						char expr[PIPE_BUF];
						strcpy(expr, buf); // copy the buffer because otherwise i had trouble
						                   // getting a correct output

						float result = getExpressionResult(buf);
						if(buf[0] == '\n') {
							close(pfds[i].fd);
							char tmpname[100];
							strcpy(tmpname, name);
							strcat(tmpname, argv[i + 1]);
							unlink(tmpname);
							printf("%s disconnected.\n", argv[i + 1]);
							current_amount_clients--;
						} else if(result != 0) {
							printf("%s: %s = %g\n", argv[i + 1], strtok(expr, "\n"), result);
						} else {
							printf("%s: %s is malformed.\n", argv[i + 1], strtok(expr, "\n"));
						}
                        break;
                        
                    default: break;
				}
			}
		} else {
			return EXIT_SUCCESS;
		}
	}
}
