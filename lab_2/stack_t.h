#ifndef __stack_t_h__
#define __stack_t_h__

#include <stdlib.h>
#include <stdio.h>

typedef struct stack_data_t {
	double a;
	double b;
	double fa;
	double fb;
	double sab;
} stack_data_t;
#define MAX_STACK_SIZE 1000
typedef struct stack_t {
	stack_data_t data [MAX_STACK_SIZE];
	size_t sp;
} stack_t;

void pop(stack_t *stack, stack_data_t *data);
void push(stack_t *stack, stack_data_t *data);

#endif
