CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -pthread -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=199309L

.PHONY: all
all: restaurant 

.PHONY: clean
clean:
	$(RM) -f restaurant

restaurant: restaurant.c
	$(CC) $(CFLAGS) -o $@ $^