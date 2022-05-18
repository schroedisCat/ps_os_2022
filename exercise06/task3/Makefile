CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -O2

.PHONY: all
all: task3 

.PHONY: clean
clean:
	$(RM) -f task3

task3: task3.c
	$(CC) $(CFLAGS) -o $@ $^