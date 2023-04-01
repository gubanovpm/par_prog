#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	int commsize, my_rank;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &commsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_File fh; MPI_Status status;
    
    int error = MPI_File_open(MPI_COMM_WORLD, "our_file.txt", MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);
    MPI_File_write(fh, )
	
	MPI_Finalize();
}
