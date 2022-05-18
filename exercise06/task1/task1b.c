#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct building_sum {
	int* num;
	int amount_entries;
	int sum;
} building_sum;

void* sum_up_numbers(void* struct_ptr);

int main(int argc, char* argv[]) {

	pthread_t ptid[argc];
	int numbers[argc];
	building_sum build_sum[argc];

	for(int i = 0; i < (argc - 1); i++) {
		numbers[i] = strtol(argv[i + 1], NULL, 10);

		build_sum[i].num = numbers;
		build_sum[i].sum = 0;
		build_sum[i].amount_entries = i + 1;

		pthread_create(&ptid[i], NULL, sum_up_numbers, &build_sum[i]);
	}

	for(int i = 0; i < (argc - 1); i++) {
		pthread_join(ptid[i], NULL);
	}

	for(int i = 0; i < (argc - 1); i++) {
		printf("sum%d = %d\n", i + 1, build_sum[i].sum);
	}
	
	return EXIT_SUCCESS;
}

void* sum_up_numbers(void* struct_ptr) {
	building_sum* bs = (building_sum*)struct_ptr;

	for(int i = 0; i < bs->amount_entries; i++) {
		bs->sum += bs->num[i];
	}

	return NULL;
}
