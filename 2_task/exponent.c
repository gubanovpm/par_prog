#include "exponent.h"

uint64_t get_borders(uint64_t last_num, int rank, int np) {
	uint32_t right_border = 1 + (last_num / np) * rank ;
	uint32_t left_border  = (rank == np - 1) ? last_num : (last_num / np) * (rank + 1);
	return ((uint64_t)left_border << 32) | right_border; 
}

uint32_t get_last(uint64_t num) {
	double xk = 2.;
	for (int i = 0 ; i < 15; ++i) {
		xk = (xk + num * log(10)) / log(xk);
	}
	return (uint32_t)xk + 10;
}
