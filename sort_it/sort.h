#ifndef __sort_h__
#define __sort_h__

#define LBMASK(_arg_) (((uint64_t)0xffffffff00000000 & _arg_) >> 32)
#define RBMASK(_arg_)  ((uint64_t)0x00000000ffffffff & _arg_)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int verify(const int *array, const size_t n, const int tos);
void random_filling(int *array, const size_t n, const size_t max);
void dump(const int *array, const size_t n);
void parallel_odd_even_sort(MPI_Comm *comm, int *array, const size_t n, const int tos);
void straight_odd_even_sort(int *array, const size_t n, const int tos);
void cmp_swap(int *first, int *second, const int tos);
uint64_t get_borders(uint64_t last_num, int rank, int np);
void split(int *f_a, const size_t nf, int *s_a, const size_t ns, const int tos);
void swap(int *first, int *second);

#endif