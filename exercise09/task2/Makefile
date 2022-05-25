CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread

.PHONY: all
all: task2_server task2_client

.PHONY: clean
clean:
	$(RM) -f task2_server task2_client

task2_server: task2_server.c
	$(CC) $(CFLAGS) -o $@ $^

task2_client: task2_client.c
	$(CC) $(CFLAGS) -o $@ $^
