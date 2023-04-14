#include "./sort.h"

#ifndef N
#define N 10000
#endif

#ifndef TOS
#define TOS 1
#endif

#ifndef MAX_NUM
#define MAX_NUM 10000
#endif

int main(int argc, char *argv[], char *envp[]) {
    MPI_Init(&argc, &argv);
    int rank, world_size; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int *array = (int *)malloc(N * sizeof(int));
    random_filling(array, N, MAX_NUM + 1);

    if (rank == 0) {
        int *sarray = (int *)malloc(N * sizeof(int));
        memcpy(sarray, array, N * sizeof(int));
        
        struct timeval stop, start;
        gettimeofday(&start, NULL);
        straight_odd_even_sort(sarray, N, TOS);
        gettimeofday(&stop, NULL);
        
        printf(
            "Is straight veryfied: %d; in time %lgs\n", 
            verify(sarray, N, TOS), 
            (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000
        );
        free(sarray);
    } else {
        MPI_Group world_group;
        MPI_Comm_group(MPI_COMM_WORLD, &world_group);
        int *ranks = (int *)malloc(world_size * sizeof(int));
        for (int i = 1; i <= world_size; ++i) {  ranks[i - 1] = i; }
        
        MPI_Group prime_group;
        MPI_Group_incl(world_group, world_size-1, ranks, &prime_group);

        MPI_Comm prime_comm;
        MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

        int prank; MPI_Comm_rank(prime_comm, &prank);
        int *parray = (int *)malloc(N * sizeof(int));
        memcpy(parray, array, N * sizeof(int));

        struct timeval stop, start;
        if (rank == 1) gettimeofday(&start, NULL);
        parallel_odd_even_sort(&prime_comm, parray, N, TOS);
        if (rank == 1) gettimeofday(&stop, NULL);

        if (rank == 1) {
            printf(
                "Is parallel veryfied: %d; in time %lgs\n", 
                verify(parray, N, TOS), 
                (double)((stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec)) / 1000000
            );
        }

        free(parray);
    }
    free(array);
    MPI_Finalize();
    return 0;
}
