CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -O2

.PHONY: all
all: task1_mutex task1_atomic

.PHONY: clean
clean:
	$(RM) -f task1_mutex task1_atomic

task1_mutex: task1.c
	$(CC) $(CFLAGS) -DUSE_MUTEX=1 -o $@ $^

task1_atomic: task1.c
	$(CC) $(CFLAGS) -DUSE_MUTEX=0 -o $@ $^


