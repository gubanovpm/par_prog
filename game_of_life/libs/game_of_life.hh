#ifndef __game_of_life_hh__
#define __game_of_life_hh__

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <mpi.h>
#include <cmath>

#define C2B (8*sizeof(unsigned char))
#define LBMASK(_arg_) (((uint64_t)0xffffffff00000000 & _arg_) >> 32)
#define RBMASK(_arg_)  ((uint64_t)0x00000000ffffffff & _arg_)

namespace GOL {

struct bit_set {
protected:
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
	bit_set() {}

	void set_ptr(unsigned char *new_ptr) {
		if      (__state == 1) delete [] __temp;
		else if (__state == 0) delete [] __data;	
		__state = 2;
		__data = new_ptr;
	}
	unsigned char *get_ptr() const { return __data;}
	~bit_set() { 
		if      (__state == 1) delete [] __temp;
		else if (__state == 0) delete [] __data;
	}

	size_t n() const noexcept { return __n; }
	size_t m() const noexcept { return __m; }
	
	bool getb(const size_t n, const size_t m) const;
	void setb(const size_t n, const size_t m, const bool state);
	void dump(std::ostream &out, const bool buf_num = 1) const noexcept;
	void rand_filling() noexcept;
};

uint64_t get_borders(uint64_t last_num, int rank, int np);

struct game_field final : public bit_set {
private:
	int __id;
	int __np;
	MPI_Comm *__comm;
	bit_set __up, __dn;

	size_t neighbours_count(const size_t i, const size_t j) const noexcept;
	void transfer() noexcept;
public:
	game_field(const size_t n, const size_t m, MPI_Comm *comm) :
		bit_set(n, m), __comm(comm) {
			MPI_Comm_size(__comm[0], &__np);
			MPI_Comm_rank(__comm[0], &__id);
			if (__np == 1) {	
				__up.set_ptr(__data + __n * (__m/C2B + __is_alignment));
				__dn.set_ptr(__data);
			} else {
				__up = {1, m}; __dn = {1, m};
			}
		}

	void transform() noexcept;
	void get_from_file(const std::string &path) noexcept;
};

};

#endif
