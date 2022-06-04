CFLAGS = -std=c11 -Wall -Wextra -Wpedantic 


.PHONY: all
all: some_library.so

.PHONY: clean
clean:
	$(RM) -f some_library.so

some_library.so: some_library.c
	$(CC) $(CFLAGS) -shared -fpic -o $@ $^  


