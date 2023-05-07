#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef THREAD_COUNT
#define THREAD_COUNT 10
#endif

void *thread_function(void *thread_arg);

int main() {
	pthread_t thid[THREAD_COUNT];
	size_t id[THREAD_COUNT];
	int result;
	for (size_t i = 0; i < THREAD_COUNT; ++i) id[i] = i;
	
	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		if (result = pthread_create(thid+i, (pthread_attr_t *)NULL, thread_function, (void *)(id + i))) {
			printf("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
	}

	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(thid[i], (void **)NULL);
	}
	return 0;
}

void *thread_function(void *thread_arg) {
	printf("Hello World, my id = %ld, thread count = %d\n", ((size_t *)thread_arg)[0], THREAD_COUNT);
}
