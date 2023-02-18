#ifndef __exponent_h__
#define __exponent_h__

#define LBMASK(_arg_) (((uint64_t)0xffffffff00000000 & _arg_) >> 32)
#define RBMASK(_arg_)  ((uint64_t)0x00000000ffffffff & _arg_)

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gmp.h>

uint64_t get_borders(uint64_t last_num, int rank, int np);
uint32_t get_last(uint64_t num);

#endif
