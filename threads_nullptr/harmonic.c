#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef THREAD_COUNT
#define THREAD_COUNT 10
#endif

void *thread_sum(void *thread_arg);

double sums[THREAD_COUNT];
typedef struct thread_arg_t {
	size_t id;
	size_t n;
} thread_arg_t;

int main(int argc, char *argv[]) {
	double s = 0;
	thread_arg_t arguments[THREAD_COUNT];
	if (argc != 2) {
		printf("Usage is : %s <n>\n", argv[0]);
		exit(-1);
	}

	pthread_t thid[THREAD_COUNT];
	int result;
	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		arguments[i].id = i, arguments[i].n = atoi(argv[1]);

		if (result = pthread_create(thid+i, (pthread_attr_t *)NULL, thread_sum, (void *)(arguments + i))) {
			printf("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
	}

	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(thid[i], (void **)NULL);
	}

	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		s += sums[i];
	}
	printf("%lg\n", s);
	return 0;
}

void *thread_sum(void *thread_arg) {
	thread_arg_t *arg = thread_arg;
	int beg_num = (arg->id == 0) ? 1 : arg->n * arg->id / THREAD_COUNT;
	int end_num = (arg->id == THREAD_COUNT - 1) ? (arg->n + 1) : (arg->n * (arg->id + 1) / THREAD_COUNT);

	for (int i = beg_num; i < end_num; ++i) 
		sums[arg->id] += 1./i;
	
	return NULL;
}
