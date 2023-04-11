#include "transfer_equation.hh"

#ifndef N
#define N 1000
#endif

#ifndef M
#define M 1000
#endif

#ifndef T
#define T 1.
#endif

#ifndef X
#define X 1.
#endif

double f(const double t, const double x) { return t + x; }
double phi(const double x) { return std::cos(M_PI * x); }
double psi(const double t) { return std::exp(-t); }

int main() {
	matrix_t data(N, M, T, X);
	simple_conveyor(f, phi, psi, data);

	auto [R, Y] = matplot::meshgrid(matplot::linspace(0, T, N), matplot::linspace(0, X, M));
	auto [I, J] = matplot::meshgrid(matplot::linspace(0, N-1, N), matplot::linspace(0, M-1, M));
	auto Z = matplot::transform(I, J, [&data](size_t k, size_t m) {
        return data(k, m);
    });

	matplot::surf(R, Y, Z);

  	matplot::show();

	return 0;
}
