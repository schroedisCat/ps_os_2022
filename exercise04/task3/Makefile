CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: task3_client task3_server

.PHONY: clean
clean:
	$(RM) -f task3_client task3_server

task3_client: task3_client.c
	$(CC) $(CFLAGS) -o $@ $^

task3_server: task3_server.c
	$(CC) $(CFLAGS) -o $@ $^
