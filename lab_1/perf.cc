#include "transfer_equation.hh"

#ifndef N
#define N 300
#endif

#ifndef T
#define T 1.
#endif

#ifndef X
#define X 1.
#endif

#ifndef PERF_SIZE_TEST
#define PERF_SIZE_TEST 16
#endif

double f(const double t, const double x) { return t + x; }
double phi(const double x) { return std::cos(M_PI * x); }
double psi(const double t) { return std::exp(-t); }

std::vector<double> s_times(PERF_SIZE_TEST);
std::vector<double> p_times(PERF_SIZE_TEST);
std::vector<size_t> sizes(PERF_SIZE_TEST);

int main(int argc, char *argv[], char *envp[]) {
	MPI_Init(&argc, &argv);
	int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int size; MPI_Comm_size(MPI_COMM_WORLD, &size); 

    MPI_Status status;
    for (int i = 0; i < PERF_SIZE_TEST; ++i) {
        sizes[i] = i;
        size_t M = size * std::pow(2, i+1);
        if (rank == 0) {
            matrix_t data(N, M, {0, T}, {0, X});
            
            struct timeval stop, start;
            gettimeofday(&start, NULL);
            simple_conveyor(f, phi, psi, data);
            gettimeofday(&stop, NULL);
            
            double ttime = (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000;
            s_times[i] = ttime;
            MPI_Recv(p_times.data() + i, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
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
            gettimeofday(&start, NULL);
            parallel_conv(f, phi, psi, data, &prime_comm);
            gettimeofday(&stop, NULL);
            double ttime = (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000;

            if (rank == 1) {
                MPI_Send(&ttime, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
        }
    }
    if (rank == 0) {
        auto p = matplot::plot(sizes, s_times, ":", sizes, p_times, "--");
        p[0]->line_width(2).color("red");
        p[1]->line_width(2).color("blue");
        matplot::show();
    }
	MPI_Finalize();
	return 0;
}
