#include "../libs/game_of_life.hh"

using namespace GOL;

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);
	game_t g {10, 10};
	g.dump(std::cout);
	MPI_Finalize();
	return 0;
}
