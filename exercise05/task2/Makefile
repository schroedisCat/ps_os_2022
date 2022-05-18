CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -D_XOPEN_SOURCE=500 -g
LDLIBS = -lpthread -lrt

.PHONY: all
all: task2

clean:
	$(RM) -f task2

task2: task2.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
