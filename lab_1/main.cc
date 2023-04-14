#include "transfer_equation.hh"

#ifndef N
#define N 3
#endif

#ifndef M
#define M 6
#endif

#ifndef T
#define T 1.
#endif

#ifndef X
#define X 1.
#endif

#define GR

#include <unistd.h>

double f(const double t, const double x) { return t + x; }
double phi(const double x) { return std::cos(M_PI * x); }
double psi(const double t) { return std::exp(-t); }

int main(int argc, char *argv[], char *envp[]) {
	MPI_Init(&argc, &argv);
	int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size; MPI_Comm_size(MPI_COMM_WORLD, &size); 

	if (rank == 0) {
		matrix_t data(N, M, {0, T}, {0, X});
		struct timeval stop, start;
    gettimeofday(&start, NULL);
		simple_conveyor(f, phi, psi, data);
		gettimeofday(&stop, NULL);
		double ttime = (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000;
		printf("Simple time is: %lg\n", ttime);
		data.dump(std::cout);
		printf("\n");
		//std::cout << "Simple time is: " << (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000 << "s" << std::endl;
	#ifdef GRAPH
		auto [R, Y] = matplot::meshgrid(matplot::linspace(0, T, N), matplot::linspace(0, X, M));
		auto [I, J] = matplot::meshgrid(matplot::linspace(0, N-1, N), matplot::linspace(0, M-1, M));
		auto Z = matplot::transform(I, J, [&data](size_t k, size_t m) {  
				return data(k, m);
		});
		//data.dump(std::cout);

	
		matplot::surf(Y, R, Z);
		matplot::colorbar();
		matplot::xlabel("X");
		matplot::ylabel("T");
		matplot::show();
	#endif
	}
	else {
		matrix_t data(N, M, {0, T}, {0, X});
		MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

		int *ranks = (int *)malloc(size * sizeof(int));
    for (int i = 1; i <= size; ++i) {  ranks[i - 1] = i; }    
    MPI_Group prime_group;
    MPI_Group_incl(world_group, size-1, ranks, &prime_group);
		free(ranks);

    MPI_Comm prime_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

    int prank; MPI_Comm_rank(prime_comm, &prank);
		int psize; MPI_Comm_size(prime_comm, &psize);

		struct timeval stop, start;
		sleep(1);
    gettimeofday(&start, NULL);
		parallel_conv(f, phi, psi, data, &prime_comm);
		gettimeofday(&stop, NULL);
		double ttime = (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000;

		if (prank == 0) {
			printf("Parallel time is: %lg\n", ttime);
			data.dump(std::cout);

			printf("\n");
/*
			for (int k = (int)data.n() - 1; k >= 0; --k) {
				for (int i = 0; i < psize; ++i) {
					for (int m = 0; m < rcounts[i]; ++m) {
						printf("%lg ", data.ptr()[k*rcounts[i]*data.n() + m]);
					}
				}
				printf("\n");
			}
*/
/*
			auto [R, Y] = matplot::meshgrid(matplot::linspace(0, T, N), matplot::linspace(0, X, M));
			auto [I, J] = matplot::meshgrid(matplot::linspace(0, N-1, N), matplot::linspace(0, M-1, M));
			auto Z = matplot::transform(I, J, [&data](size_t k, size_t m) { return data(k, m); });
			data.dump(std::cout);

			matplot::surf(Y, R, Z);
			matplot::colorbar();
			matplot::xlabel("X");
			matplot::ylabel("T");
			matplot::show();
*/
		}
	}
	MPI_Finalize();
	return 0;
}
