CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -D_DEFAULT_SOURCE -pthread

.PHONY: all
all: task1 

.PHONY: clean
clean:
	$(RM) -f task1

task1: task1.c allocator_tests.c membench.c
	$(CC) $(CFLAGS) -o $@ $^

