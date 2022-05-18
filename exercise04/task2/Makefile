CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: ls_wc ls_grep_wc

.PHONY: clean
clean:
	$(RM) -f ls_wc ls_grep_wc

ls_wc: ls_wc.c
	$(CC) $(CFLAGS) -o $@ $^

ls_grep_wc: ls_grep_wc.c
	$(CC) $(CFLAGS) -o $@ $^
