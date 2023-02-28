#include "exponent.h"

int main(int argc, char *argv[]) {
	int comsize, rank;
	uint32_t precision;

	if (argc != 2) { printf("Wrong argument count.\nPlease check your input.\n"); exit(-1); }
	if (!(precision = atol(argv[1]))) {"Wrong argument.\nExpected number uint32_t type.\nPlease try again\n"; exit(-1); }
	uint32_t last_factorial = get_last(precision);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &comsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	uint64_t borders = get_borders(last_factorial, rank, comsize);
	mpz_t sum ; mpz_init(sum); 
	mpz_t prod; mpz_init(prod); mpz_add_ui(prod, prod, 1);

	for (uint32_t cur = LBMASK(borders), end = RBMASK(borders); cur >= end; --cur) {
		mpz_mul_ui(prod, prod, cur);
		mpz_add   (sum, sum, prod);
	}
	
	mpz_t fact; mpz_init(fact);

	if (rank != 0) {
		size_t len = mpz_sizeinbase(prod, 10);
		char *buf = (char *)calloc(len+1, sizeof(char));
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		len = gmp_sprintf(buf, "%Zd", prod);
		for (int i = rank - 1; i >= 0; --i) {
			MPI_Send(&len,   1,  MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send( buf, len, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		free(buf);

	} else {
		mpz_add(fact, fact, prod);
	}
	for (int i = rank + 1; i < comsize; ++i) {
		int len = 0; MPI_Status status;
		MPI_Recv(&len,   1,  MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		char *buf = (char *)calloc(len+1, sizeof(char));
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);
		MPI_Recv( buf, len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		gmp_sscanf(buf, "%Zd", &prod);
		mpz_mul(sum, sum, prod);
		if (rank == 0) mpz_mul(fact, fact, prod);
		free(buf);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	char *buf; 
	if (rank != 0) {
		size_t len = mpz_sizeinbase(sum, 10);
		buf = (char *)calloc(len+1, sizeof(char));
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		len = gmp_sprintf(buf, "%Zd", sum);
		MPI_Send(&len,   1,  MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send( buf, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	} else {
		for (int i = 1; i < comsize; ++i) {
			int len = 0; MPI_Status status;
			MPI_Recv(&len,   1,  MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			buf = (char *)calloc(len+1, sizeof(char));
			if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);
			MPI_Recv( buf, len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			gmp_sscanf(buf, "%Zd", &prod);
			mpz_add(sum, sum, prod);
			free(buf);
		}
		mpz_add_ui(sum, sum, 1);
		
		uint32_t prec_b = precision * 8;
		mpf_t result; mpf_init(result); mpf_set_prec(result, prec_b);
		mpf_t first ; mpf_init( first); mpf_set_prec( first, prec_b);
		mpf_t second; mpf_init(second); mpf_set_prec(second, prec_b);
		mpf_set_z(first, sum); mpf_set_z(second, fact);
		mpf_div(result, first, second);
		gmp_printf("%.*Ff\n", precision - 1, result);

	}
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}
