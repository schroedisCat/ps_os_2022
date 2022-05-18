CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -D_XOPEN_SOURCE=500 -g
LDLIBS = -lrt

.PHONY: all
all: task1

.PHONY: clean
clean:
	$(RM) -f task1

task1: task1.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
