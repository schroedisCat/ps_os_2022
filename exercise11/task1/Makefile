CFLAGS = -std=c11 -Wall -Wextra -Wpedantic 


.PHONY: all
all: my_program some_library.so

.PHONY: clean
clean:
	$(RM) -f some_library.so my_program

some_library.so: some_library.c
	$(CC) $(CFLAGS) -shared -fpic -o $@ $^ 

my_program: my_program.c
	$(CC) $(CFLAGS) -static -o $@ $^ 


