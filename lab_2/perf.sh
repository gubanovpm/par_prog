#!/bin/bash

for ((i = 1; i < 13; i++))
do
	gcc ../parallel.c ../stack_t.c -lm -DTHREAD_COUNT=${i} -o parallel
	./parallel
done
