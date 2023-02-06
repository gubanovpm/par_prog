#include <mpi.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
	int commsize, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int message = 1;
	if (rank == 0) {
		printf("Proc number %d value is = %d\n", rank, message);
		if (commsize > 1)
			MPI_Send(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Status status;
		MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		if (status.MPI_SOURCE == commsize - 1)
			printf("I\'m done\n");
	} else {	
		MPI_Recv(&message, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		message += 1;
		printf("Proc number %d value is = %d\n", rank, message);
		
		int dest = 0;
		if (rank != commsize - 1) dest = rank + 1;
		MPI_Send(&message, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
