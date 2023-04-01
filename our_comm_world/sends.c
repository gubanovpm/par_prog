#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <mpi.h>

#ifndef SEND_T
    #define SEND MPI_Send
#elif  (SEND_T == S)
    #define SEND MPI_Ssend
#elif  (SEND_T == R)
    #define SEND MPI_Rsend
#elif  (SEND_T == B)
    #define SEND MPI_Bsend
#endif

int main(int argc, char *argv[], char *envp[]) {
    int N = 0;
	if (argc != 2) {
		printf("Wrong argument's size, please check your input\n");
		exit(-1);
	}
	N = atoi(argv[1]);

	MPI_Init(&argc, &argv);

	int world_size = 0, world_rank = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_size != 2) {
		printf("Wrong size of COMM_WORLD!\nAlways two, there are. No more. No less. A Master and an apprentice.\n");
		return 0;
	}

	char *array = (char *)calloc(N, sizeof(char));
	struct timeval stop, start;
    
    if (!world_rank) {
        gettimeofday(&start, NULL);
        sleep(2);
        gettimeofday(&stop, NULL);

        MPI_Status status;
        MPI_Recv(array, N, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);

        printf("Recieve : %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
    } else {
        gettimeofday(&start, NULL);
        SEND(array, N, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        gettimeofday(&stop, NULL);

        printf("Send    : %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec));
    }

    free(array);
	MPI_Finalize();
	return 0;
}