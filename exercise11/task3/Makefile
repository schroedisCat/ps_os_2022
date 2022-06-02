CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g


.PHONY: all
all: malloc_spy.so

.PHONY: clean
clean:
	$(RM) -f malloc_spy.so

malloc_spy.so: malloc_spy.c
	$(CC) $(CFLAGS) -shared -fpic -o $@ $^
