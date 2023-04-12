#include "transfer_equation.hh"

#ifndef N
#define N 10
#endif

#ifndef M
#define M 10
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

int main(int argc, char *argv[], char *envp[]) {
	MPI_Init(&argc, &argv);
	int rank = 0; MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size = 0; MPI_Comm_size(MPI_COMM_WORLD, &size); 

	if (rank == 0) {
		matrix_t data(N, M, T, X);
		simple_conveyor(f, phi, psi, data);

		auto [R, Y] = matplot::meshgrid(matplot::linspace(0, T, N), matplot::linspace(0, X, M));
		auto [I, J] = matplot::meshgrid(matplot::linspace(0, N-1, N), matplot::linspace(0, M-1, M));
		auto Z = matplot::transform(I, J, [&data](size_t k, size_t m) {  
				return data(k, m);
		});
		//data.dump(std::cout);

		//matplot::surf(Y, R, Z);
		//matplot::colorbar();
		//matplot::xlabel("X");
		//matplot::ylabel("T");
		//matplot::show();
	}
	else {
		matrix_t data(N, M, T, X);
		MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    int *ranks = (int *)malloc(size * sizeof(int));
    for (int i = 1; i <= size; ++i) {  ranks[i - 1] = i; }
        
    MPI_Group prime_group;
    MPI_Group_incl(world_group, size-1, ranks, &prime_group);

    MPI_Comm prime_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

    int prank; MPI_Comm_rank(prime_comm, &prank);
		parallel_conv(f, phi, psi, data, &prime_comm);
		MPI_Comm_rank(prime_comm, &rank);
		if (rank == 0) {
			data.dump(std::cout);
					auto [R, Y] = matplot::meshgrid(matplot::linspace(0, T, N), matplot::linspace(0, X, M));
			auto [I, J] = matplot::meshgrid(matplot::linspace(0, N-1, N), matplot::linspace(0, M-1, M));
			auto Z = matplot::transform(I, J, [&data](size_t k, size_t m) {  
					return data(k, m);
			});
			data.dump(std::cout);

			matplot::surf(Y, R, Z);
			matplot::colorbar();
			matplot::xlabel("X");
			matplot::ylabel("T");
			matplot::show();

		}
	}
	MPI_Finalize();
	return 0;
}
