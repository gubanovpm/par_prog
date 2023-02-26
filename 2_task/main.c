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
	//printf("rank = %d, leftb = %lu, rightb = %lu\n", rank, LBMASK(borders), RBMASK(borders));
  mpz_t sum ; mpz_init(sum); 
	mpz_t prod; mpz_init(prod); mpz_add_ui(prod, prod, 1);

	for (uint32_t cur = LBMASK(borders), end = RBMASK(borders); cur >= end; --cur) {
		//gmp_printf("%Zd\n", sum);
		mpz_mul_ui(prod, prod, cur);
		mpz_add   (sum, sum, prod);
		//gmp_printf("%Zd\n", sum);
	}
	//gmp_printf("lb = %lu | rb = %lu | rank is %d | sum is = %Zd | prod is = %Zd\n", LBMASK(borders), RBMASK(borders), rank, sum, prod);
	// now we want to send multipl to other process
	
	mpz_t fact; mpz_init(fact);

	if (rank != 0) {
		size_t len = mpz_sizeinbase(prod, 10);
		char *buf; MPI_Alloc_mem(len+1, MPI_INFO_NULL, buf);
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		len = gmp_sprintf(buf, "%Zd", prod);
		printf("%s\n", buf);
		for (int i = rank - 1; i >= 0; --i) {
			MPI_Send(&len,   1,  MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send( buf, len, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		MPI_Free_mem(buf);

	} else {
		mpz_add(fact, fact, prod);
	}
	for (int i = rank + 1; i < comsize; ++i) {
		int len = 0; MPI_Status status;
		MPI_Recv(&len,   1,  MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		printf("len = %d\n", len);
		char *buf; MPI_Alloc_mem(len+1, MPI_INFO_NULL, buf);
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);
		MPI_Recv( buf, len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
		printf("Recieve number is %s\n", buf);
		gmp_sscanf(buf, "%Zd", &prod);
		mpz_mul(sum, sum, prod);
		if (rank == 0) mpz_mul(fact, fact, prod);
		MPI_Free_mem(buf);
	}
	//gmp_printf("My rank = %d | sum = %Zd\n", rank, sum);
	MPI_Barrier(MPI_COMM_WORLD);

	char *buf; 
	if (rank != 0) {
		size_t len = mpz_sizeinbase(sum, 10);
		MPI_Alloc_mem(len + 1, MPI_INFO_NULL, buf);
		if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);

		len = gmp_sprintf(buf, "%Zd", sum);
		//gmp_printf("My rank %d and I send %Zd\n", rank, sum);
		MPI_Send(&len,   1,  MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send( buf, len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	} else {
		for (int i = 1; i < comsize; ++i) {
			int len = 0; MPI_Status status;
			MPI_Recv(&len,   1,  MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			char *buf; MPI_Alloc_mem(len + 1, MPI_INFO_NULL, buf);
			if (!buf) MPI_Abort(MPI_COMM_WORLD, 911);
			MPI_Recv( buf, len, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
			gmp_sscanf(buf, "%Zd", &prod);
			mpz_add(sum, sum, prod);
			MPI_Free_mem(buf);
			//gmp_printf("When I add form %d prod = %Zd sum is : %Zd\n", i, prod, sum);
		}
		mpz_add_ui(sum, sum, 1);
		
		uint32_t prec_b = precision * 8;
		mpf_t result; mpf_init(result); mpf_set_prec(result, prec_b);
		mpf_t first ; mpf_init( first); mpf_set_prec( first, prec_b);
		mpf_t second; mpf_init(second); mpf_set_prec(second, prec_b);
		mpf_set_z(first, sum); mpf_set_z(second, fact);
		//gmp_printf("factorial is : %Ff\n", second);
		//gmp_printf("summary is   : %Ff\n",  first);
		mpf_div(result, first, second);
		gmp_printf("%.*Ff\n", precision - 1, result);

	}
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return 0;
}
