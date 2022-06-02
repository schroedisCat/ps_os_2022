#include <stdio.h>
#include <stdlib.h>

int square_my_integer(int x) {
	return (x * x);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("the command should look like: ./my_programm <number>\n");
        exit(EXIT_FAILURE);
    }

    int num = strtol(argv[1], NULL, 10);
    printf("%d\n",square_my_integer(num));


    return EXIT_SUCCESS;
}