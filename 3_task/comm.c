#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[], char *envp[]) {
  int N = 0;
	if (argc != 2) {
		printf("Wrong argument's size, please check your input\n");
		exit(-1);
	}
	N = atoi(argv[1]);
  MPI_Init(NULL, NULL);

  // Get the rank and size in the original communicator
  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the group of processes in MPI_COMM_WORLD
  MPI_Group world_group;
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);

  int *ranks = (int *)calloc(world_size, sizeof(int));
  for (int i = 1; i <= world_size; ++i) {  ranks[i - 1] = i; }

  // Construct a group containing all of the prime ranks in world_group
  MPI_Group prime_group;
  MPI_Group_incl(world_group, world_size-1, ranks, &prime_group);

  // Create a new communicator based on the group
  MPI_Comm prime_comm;
  MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

  int prime_rank = -1, prime_size = -1;
  // If this rank isn't in the new communicator, it will be MPI_COMM_NULL
  // Using MPI_COMM_NULL for MPI_Comm_rank or MPI_Comm_size is erroneous
  if (MPI_COMM_NULL != prime_comm) {
    MPI_Comm_rank(prime_comm, &prime_rank);
    MPI_Comm_size(prime_comm, &prime_size);
  }

  printf("WORLD RANK/SIZE: %d/%d --- PRIME RANK/SIZE: %d/%d\n", world_rank, world_size, prime_rank, prime_size);

  if (prime_rank != -1) {
    double sum = 0., res = 0.;
    
    int beg_num = (prime_rank == 0) ? 1 : N * prime_rank / prime_size;
  	int end_num = (prime_rank == prime_size - 1) ? (N + 1) : (N * (prime_rank + 1) / prime_size);
    for (int i = beg_num; i < end_num; ++i) {
		  sum += 1. / i;
	  }

    MPI_Reduce(&sum, &res, 1, MPI_DOUBLE, MPI_SUM, 0, prime_comm);
    if (prime_rank == 0) {
      printf("Result is = %lg\n", res);
    }
  }
  
  free(ranks);

  MPI_Group_free(&world_group);
  MPI_Group_free(&prime_group);

  if (MPI_COMM_NULL != prime_comm) {
    MPI_Comm_free(&prime_comm);
  }

  MPI_Finalize();
}