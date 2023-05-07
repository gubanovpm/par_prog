#include "parametrs.h"
#include "stack_t.h"

void *thread_func(void *dummy);

double s = 0;
size_t nactive = 0;
sem_t gstack_empty, gstack_tasks, is_summing;
stack_t gstack;

int main() {
	gstack.sp = 0;
	if (sem_init(&gstack_empty, 0, 1)) {
		printf("Error on init semaphore, return value = %d\n", errno);
		exit(-1);
	}
	if (sem_init(&gstack_tasks, 0, 1)) {
		printf("Error on init semaphore, return value = %d\n", errno);
		exit(-1);
	}
	if (sem_init(&is_summing, 0, 1)) {
		printf("Error on init semaphore, return value = %d\n", errno);
		exit(-1);
	}
	
	stack_data_t temp = {BEG, END, F(BEG), F(END), (F(END)+F(BEG))*(END-BEG)/2};
	push(&gstack, &temp);
	pthread_t thid[THREAD_COUNT];
	int result;
	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		if (result = pthread_create(thid + i, (pthread_attr_t *)NULL, thread_func, (void *)NULL)) {
			printf("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
	}
		#ifdef HAVE_THR_SETCONCURRENCY_PROTO
			thr_setconcurrency(THREAD_COUNT);
		#endif

	for (size_t i = 0; i < THREAD_COUNT; ++i) {
		pthread_join(thid[i], (void **)NULL);
	}

	sem_destroy(&gstack_empty);
	sem_destroy(&gstack_tasks);
	sem_destroy(&is_summing);
	printf("Integral is = %lg\n", s);

	return 0;
}

void *thread_func(void *dummy) {
	stack_t lstack;
	lstack.sp = 0;
	double ls = 0;
	stack_data_t temp;
	while (1) {
		sem_wait(&gstack_tasks);
		sem_wait(&gstack_empty);

		pop(&gstack, &temp);

		if (gstack.sp) sem_post(&gstack_tasks);
		if (temp.a <= temp.b) ++nactive;

		sem_post(&gstack_empty);
		if (temp.a > temp.b) break;

		while (1) {
			double c  = (temp.a + temp.b) / 2;
			double fc = F(c);
		
			double sac = (temp.fa + fc) * (c - temp.a) / 2;
			double scb = (temp.fb + fc) * (temp.b - c) / 2;
			double sacb = sac + scb;

			if (fabs(temp.sab - sacb) < EPS) {
				ls += sacb;
				if (!lstack.sp) break;
				pop(&lstack, &temp);
			} else {
				stack_data_t t = {temp.a, c, temp.fa, fc, sac};
				push(&lstack, &t);
				temp.a = c;
				temp.fa = fc;
				temp.sab = scb;
			}

			if ((lstack.sp > SPK) && (!gstack.sp)) {
				sem_wait(&gstack_empty);
				if (!gstack.sp) sem_post(&gstack_tasks);
				while ((lstack.sp > 1) && (gstack.sp + (size_t)1 < MAX_STACK_SIZE)) {
					stack_data_t t;
					pop (&lstack, &t);
					push(&gstack, &t);
				}
				sem_post(&gstack_empty);
			}
		}
		sem_wait(&gstack_empty);
		--nactive;

		if ((!nactive) && (!gstack.sp)) {
			for (size_t i = 0; i < THREAD_COUNT; ++i) {
				temp.a = 2, temp.b = 1;
				push(&gstack, &temp);
			}
			sem_post(&gstack_tasks);
		}
		sem_post(&gstack_empty);
	}

	sem_wait(&is_summing);
	s += ls;
	sem_post(&is_summing);

	return NULL;
}
