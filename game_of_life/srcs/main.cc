#include "../libs/game_of_life.hh"

using namespace GOL;

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);
	int size; MPI_Comm_size(MPI_COMM_WORLD, &size); 	
	MPI_Group world_group;
  MPI_Comm_group(MPI_COMM_WORLD, &world_group);
	
	int *ranks = (int *)malloc(size * sizeof(int));
  for (int i = 0; i < size; ++i) {  ranks[i] = i; }    
	MPI_Group prime_group;
	MPI_Group_incl(world_group, size, ranks, &prime_group);
	free(ranks);

	MPI_Comm prime_comm;
	MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

	game_field g {5, 5, &prime_comm};
	g.rand_filling();
	g.dump(std::cout);
	std::cout << std::endl;
	for (int i = 0; i < 4; ++i) {
		g.transform();
		g.dump(std::cout);
		std::string s;
		std::cout << std::endl;
	}
	//g.dump(std::cout);
	MPI_Finalize();
	return 0;
}
