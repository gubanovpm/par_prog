#include "transfer_equation.hh"

double cross_scheme(const ftype &f, const matrix_t &data, const size_t k, const size_t m) {
	size_t mn   = ((m == data.m() - 1) ? m : m+1);
	size_t kn_1 = ((k == 0) ? 0 : k-1 );
	double tmp = 2*data.tau()*(f(data.tau() * k, data.h() * m) - (data(k, mn) - data(k, m-1)) / (2 * data.h())) + data(kn_1, m);
	return tmp;
}

void simple_conveyor(const ftype &f, const utype &phi, const utype &psi, matrix_t &data) {
	for (size_t i = 0; i < data.n(); ++i) data(i, 0) = psi( data.tau() * i );
	for (size_t j = 0; j < data.m(); ++j) data(0, j) = phi( data.h() * j );
	
	for (size_t k = 1; k < data.n(); ++k) {
		for (size_t m = 1; m < data.n(); ++m) {
			data(k, m) = cross_scheme(f, data, k-1, m);
		}
	}
	
}
