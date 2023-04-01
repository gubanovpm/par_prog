#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int N = 0;
	if (argc != 2) {
		printf("Wrong argument's size, please check your input\n");
		exit(-1);
	}
	N = atoi(argv[1]);

	int commsize, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int beg_num = (rank == 0) ? 1 : N * rank / commsize;
	int end_num = (rank == commsize - 1) ? (N + 1) : (N * (rank + 1) / commsize);

	if (beg_num == 0) beg_num = 1;
	double sum = 0;
	for (int i = beg_num; i < end_num; ++i) {
		sum += 1. / i;
	}

	if (rank != 0) {
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	} else {
		double temporary = 0;
		for (int i = 1; i < commsize; ++i) {
			MPI_Recv(&temporary, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += temporary;
		}
		printf("Summary is = %lg\n", sum);
	}

	MPI_Finalize(); 
	return 0;
}
