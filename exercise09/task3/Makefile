CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread

.PHONY: all
all: task3_server task3_client

.PHONY: clean
clean:
	$(RM) -f task3_server task3_client

task3_server: task3_server.c
	$(CC) $(CFLAGS) -o $@ $^

task3_client: task3_client.c
	$(CC) $(CFLAGS) -o $@ $^