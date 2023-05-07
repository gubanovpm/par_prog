#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef THREAD_COUNT
#define THREAD_COUNT 10
#endif

int s  = 0;
int id = 0;

void *snowball(void *arg);

int main() {
	pthread_t thid[THREAD_COUNT];
	int result;
	
	int th_id[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; ++i) th_id[i] = i;

	for (int i = 0; i < THREAD_COUNT; ++i) {
		if (result = pthread_create(thid+i, (pthread_attr_t *)NULL, snowball, (void *)(th_id + i))) {
			printf("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
	}

	for (int i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(thid[i], (void **)NULL);
	}
	return 0;
}

void *snowball(void *arg) {
	while (id != ((int *)arg)[0]);
	
	s += 1;	
	printf("My id = %d | glob var is = %d\n", id, s);
	++id;	
	
	return NULL;
}
