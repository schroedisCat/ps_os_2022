CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: task2

.PHONY: clean
clean:
	$(RM) -f task2

task2: task2.c
	$(CC) $(CFLAGS) -o $@ $^
