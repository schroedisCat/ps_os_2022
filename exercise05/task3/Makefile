CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -D _BSD_SOURCE -D_POSIX_C_SOURCE
LDLIBS = -lrt

.PHONY: all
all: task3_printer_server task3_printer

clean:
	$(RM) -f task3_printer_server task3_printer

task3_printer_server: task3_printer_server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

task3_print: task3_printer.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS) 