#include "stack_t.h"

void pop(stack_t *stack, stack_data_t *data) {
	if (stack->sp == 0) printf("Trying to get element from empty stack\n"), exit(-1);
	*data = stack->data[--stack->sp];
}

void push(stack_t *stack, stack_data_t *data) {
	if (stack->sp == MAX_STACK_SIZE) printf("Trying to push when stack is full\n"), exit(-1);
	stack->data[stack->sp++] = *data;
}

