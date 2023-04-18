#include "transfer_equation.hh"

#ifndef N
#define N 100
#endif

#ifndef M
#define M 100
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
	int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size; MPI_Comm_size(MPI_COMM_WORLD, &size); 

	if (rank == 0) {
		matrix_t data(N, M, {0, T}, {0, X});
		
		struct timeval stop, start;
    	gettimeofday(&start, NULL);
		simple_conveyor(f, phi, psi, data);
		gettimeofday(&stop, NULL);
		
		double ttime = (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000;
		printf("Simple: %lg\n", ttime);
	
		#ifdef GRAPHICS
			draw_graph("simple.png", data);
		#endif
	}
	else {
		printf("%d\n", M);
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
    	gettimeofday(&start, NULL);
		parallel_conv(f, phi, psi, data, &prime_comm);
		gettimeofday(&stop, NULL);
		double ttime = (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000;

		if (prank == 0) {
			printf("Parallel: %lg\n", ttime);
			
			#ifdef GRAPHICS
				draw_graph("parallel.png", data);
			#endif
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
