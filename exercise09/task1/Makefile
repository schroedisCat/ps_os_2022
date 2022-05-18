CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread

.PHONY: all
all: task1_server task1_client

.PHONY: clean
clean:
	$(RM) -f task1_server task1_client

task1_server: task1_server.c
	$(CC) $(CFLAGS) -o $@ $^

task1_client: task1_client.c
	$(CC) $(CFLAGS) -o $@ $^
