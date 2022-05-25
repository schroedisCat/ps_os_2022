CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: task1

.PHONY: clean
clean:
	$(RM) -f task1

task1: task1.c
	$(CC) $(CFLAGS) -o $@ $^
