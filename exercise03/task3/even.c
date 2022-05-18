#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	// return 2 if no or more than one argument was provided
	if(argc != 2) {
		return 2;
	}

	// Special case if number is 0
	if(strcmp(argv[1], "0") == 0) {
		return 0;
	}
	// atoi returns 0 if no conversion can be performed
	const int number = atoi(argv[1]);
	if(number != 0) {
		// return 1 if the first argument is an odd number
		if(number % 2 == 1) {
			return 1;
		}
		// return 0 if argv[1] is an even number
		return 0;
	}
	// return 3 if first argument is not a number
	return 3;
}