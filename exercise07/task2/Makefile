CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -O2 -g

.PHONY: all
all: task2_mutex task2_atomic

.PHONY: clean
clean:
	$(RM) -f task2_mutex task2_atomic 

task2_mutex: task2.c
	$(CC) $(CFLAGS) -DUSE_MY_MUTEX=0 -o $@ $^

task2_atomic: task2.c
	$(CC) $(CFLAGS) -DUSE_MY_MUTEX=1 -o $@ $^ 



