#ifndef __transfer_equation_h__
#define __transfer_equation_h__

#ifndef LBMASK
#define LBMASK(_arg_) (((uint64_t)0xffffffff00000000 & _arg_) >> 32)
#endif

#ifndef RBMASK
#define RBMASK(_arg_)  ((uint64_t)0x00000000ffffffff & _arg_)
#endif

#include <iostream>
#include <cstring>
#include <functional>
#include <cmath>
#include <sys/time.h>
#include <time.h>
#include <iomanip>

#include <mpi.h>
#include <matplot/matplot.h>

using utype = std::function<double(const double)>;
using ftype = std::function<double(const double, const double)>;

struct matrix_t {
private:
	size_t _n = 0;
	size_t _m = 0;	
	double _tau = 0;
	double _h   = 0;
	double *_data  = nullptr;
public:
	size_t n ()  const noexcept { return _n; };
	size_t m ()  const noexcept { return _m; };
	double tau() const noexcept { return _tau; };
	double h()   const noexcept { return _h; };
	double *ptr() const noexcept { return _data; }

	double &operator() (const size_t i, const size_t j) const { 
		if (i >= _n) throw std::invalid_argument("Wrong first size argument value\n");
		if (j >= _m) throw std::invalid_argument("Wrong second size argument value\n");
		return _data[i * _m + j]; 
	}
	double &operator() (const size_t k) const {
		if (k >= _n * _m) throw std::invalid_argument("Wrong buffer size\n");
		return _data[k];
	}
	matrix_t(
		const size_t n, 
		const size_t m, 
		const std::pair<double, double> &T, 
		const std::pair<double, double> &X):
			_n(n), _m(m) {
				_data = new double [n * m]{};
				_tau  = (T.second - T.first) / _n;
				_h    = (X.second - X.first) / _m;
		}
	void dump(std::ostream &out) {
		for (size_t i = _n; i > 0; --i) {
			for (size_t j = 0; j < _m; ++j) {
				out << std::setw(4) << std::setprecision(4) << _data[(i - 1) * _m + j] << " ";
			}
			out << std::endl;
		}
	}
	~matrix_t() {
		delete [] _data;
	}
};

void simple_conveyor(const ftype &f, const utype &phi, const utype &psi, matrix_t &data);
void parallel_conv(const ftype &f, const utype &phi, const utype &psi, matrix_t &data, MPI_Comm *comm);
double cross_scheme(const ftype &f, const matrix_t &data, const size_t n, const size_t m);
uint64_t get_borders(uint64_t last_num, int rank, int np);

#endif
