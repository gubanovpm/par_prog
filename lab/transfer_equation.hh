#ifndef __transfer_equation_h__
#define __transfer_equation_h__

#include <iostream>
#include <functional>
#include <cmath>

#include <matplot/matplot.h>

using utype = std::function<double(const double)>;
using ftype = std::function<double(const double, const double)>;

struct matrix_t {
private:
	size_t _n = 0;
	size_t _m = 0;
	double *_data  = nullptr;
	double _tau = 0;
	double _h   = 0;
public:
	size_t n ()  const noexcept { return _n; };
	size_t m ()  const noexcept { return _m; };
	double tau() const noexcept { return _tau; };
	double h()   const noexcept { return _h; };

	double &operator() (const size_t i, const size_t j) const { 
		if (i >= _n) throw std::invalid_argument("Wrong first size argument value\n");
		if (j >= _m) throw std::invalid_argument("Wrong second size argument value\n");
		return _data[i * _m + j]; 
	}
	double &operator() (const size_t k) const {
		if (k >= _n * _m) throw std::invalid_argument("Wrong buffer size\n");
		return _data[k];
	}
	matrix_t(const size_t n, const size_t m, const double _T, const double _X):
		_n(n), _m(m) {
			_data = new double [n * m]{};
			_tau  = _T / _n;
			_h    = _X / _m;
		}
	void dump(std::ostream &out) {
		for (size_t i = _n; i > 0; --i) {
			for (size_t j = 0; j < _m; ++j) {
				out << _data[(i - 1) * _m + j] << " ";
			}
			out << std::endl;
		}
	}
};

void simple_conveyor(const ftype &f, const utype &phi, const utype &psi, matrix_t &data);
double cross_scheme(const ftype &f, const matrix_t &data, const size_t n, const size_t m);

#endif
