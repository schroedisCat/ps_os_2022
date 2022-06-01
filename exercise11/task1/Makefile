CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -shared -fpic


.PHONY: all
all: some_library.so  

.PHONY: clean
clean:
	$(RM) -f some_library.so 

some_library.so: some_library.c
	unset LD_LIBRARY_PATH
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
	$(CC) $(CFLAGS) -o $@ $^ 

