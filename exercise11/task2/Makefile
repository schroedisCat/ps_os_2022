CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g


.PHONY: all
all: square.so negate.so double.so map_number

.PHONY: clean
clean:
	$(RM) -f map_number square.so negate.so double.so

square.so: square.c
	$(CC) $(CFLAGS) -shared -fpic -o $@ $^

negate.so: negate.c
	$(CC) $(CFLAGS) -shared -fpic -o $@ $^

double.so: double.c
	$(CC) $(CFLAGS) -shared -fpic -o $@ $^ 

map_number: map_number.c
	$(CC) $(CFLAGS) -o $@ $^  -ldl 

