#ifndef __game_of_life_hh__
#define __game_of_life_hh__

#include <iostream>
#include <cstdlib>
#include <mpi.h>

#define C2B (8*sizeof(unsigned char))
#define LBMASK(_arg_) (((uint64_t)0xffffffff00000000 & _arg_) >> 32)
#define RBMASK(_arg_)  ((uint64_t)0x00000000ffffffff & _arg_)

namespace GOL {

struct bit_set final {
private:
	unsigned char *__data = nullptr;
	unsigned char *__temp = nullptr;
	size_t __n;
	size_t __m;	
	size_t __is_alignment;
	size_t __state;

	bool getn_in_el(size_t n, char el) const noexcept;
	void dump(const char el, std::ostream &out, const size_t count = C2B) const noexcept;
	void swap_bufp() noexcept;
public:
	bit_set(const size_t n, const size_t m):
		__n(n), __m(m), __state(0) {
			__is_alignment = !!(m % C2B);
			__data = new unsigned char [2*n*(m/C2B + __is_alignment)]{};
			__temp = __data + __n*(__m/C2B + __is_alignment);
		}
	~bit_set() { 
		if (__state) delete [] __temp;
		else         delete [] __data;
	}

	size_t n() const noexcept { return __n; }
	size_t m() const noexcept { return __m; }
	
	bool getb(const size_t n, const size_t m) const;
	void setb(const size_t n, const size_t m, const bool state);
	void dump(std::ostream &out, const bool buf_num = 1) const noexcept;
	void rand_filling();
};

struct game_t final {
private:
	int __id;
	int __np;
	MPI_Comm __comm;
	size_t *__rcounts;
	size_t *__displs;
	bit_set *__data = nullptr;

	uint64_t get_borders(uint64_t last_num, int rank, int np);
	size_t neighbours_count(const size_t i, const size_t j) const noexcept;
public:
	game_t(const size_t n, const size_t m) {
		__comm = MPI_COMM_WORLD;
		MPI_Comm_size(__comm, &__np);
		MPI_Comm_rank(__comm, &__id);
		__rcounts = new size_t [__np]{};
		__displs  = new size_t [__np]{};
		for (int i = 0; i < __np; ++i) {
			uint64_t borders = get_borders(n, i, __np);
			__displs [i] = LBMASK(borders) - 1;
			__rcounts[i] = RBMASK(borders);
		}
		__data = new bit_set {__rcounts[__id] + 2, m};
		}
	~game_t() {
		delete [] __rcounts;
		delete [] __displs;
		delete __data;
	}

	void dump(std::ostream &out, const bool buf_num = 1) const noexcept;
	void transform() noexcept;
};

};

#endif
