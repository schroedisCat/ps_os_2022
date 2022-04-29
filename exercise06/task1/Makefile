CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread

.PHONY: all
all: task1a task1b

.PHONY: clean
clean:
	$(RM) -f task1a task1b

task1a: task1a.c
	$(CC) $(CFLAGS) -o $@ $^

task1b: task1b.c
	$(CC) $(CFLAGS) -o $@ $^

