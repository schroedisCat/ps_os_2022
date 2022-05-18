CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -O2

.PHONY: all
all: task1a task1b

.PHONY: clean
clean:
	$(RM) -f task1a task1b 

task1a: task1.c
	$(CC) $(CFLAGS) -DUSE_THREAD_POOL=0 -o $@ $^

task1b: task1.c
	$(CC) $(CFLAGS) -DUSE_THREAD_POOL=1 -o $@ $^




