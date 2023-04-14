#include "transfer_equation.hh"

double cross_scheme(const ftype &f, const matrix_t &data, const size_t k, const size_t m) {
	size_t mn   = ((m == data.m() - 1) ? m : m+1);
	size_t kn_1 = ((k == 0) ? 0 : k-1 );
	return 2*data.tau()*(f(data.tau()*k, data.h()*m) - ((data(k,mn) - data(k,m-1)) / (2 * data.h()))) + data(kn_1, m);
}

double cross_scheme(const ftype &f, const matrix_t &data, const size_t offset, const size_t k, const size_t m, const std::pair<double, double> &ot_val) {
	double pvalue, nvalue;
	double tau = data.tau(), h = data.h();
	size_t kn_1 = ((k == 0) ? 0 : k-1);

	if (m   == 0) pvalue = ot_val.first;
	else  pvalue = data(kn_1, m);
	if (m+1 == data.m()) nvalue = ot_val.second;
	else nvalue = data(k, m+1);

	return 2*tau*(f(tau*k, h*(m+offset)) - (nvalue - pvalue)/(2*h)) + data(kn_1, m);
}

uint64_t get_borders(uint64_t last_num, int rank, int np) {
	uint32_t right_border = 1 + (last_num / np) * rank ;
	uint32_t left_border  = (rank == np - 1) ? last_num : (last_num / np) * (rank + 1);
		uint32_t temporary = left_border - right_border;
		temporary = ((temporary == 0) ? 0 : temporary + 1); 
	return ((uint64_t)right_border << 32) | (temporary); 
}

void simple_conveyor(const ftype &f, const utype &phi, const utype &psi, matrix_t &data) {
	for (size_t i = 0; i < data.n(); ++i) data(i, 0) = psi( data.tau() * i );
	for (size_t j = 0; j < data.m(); ++j) data(0, j) = phi( data.h() * j );

	for (size_t k = 1; k < data.n(); ++k) {
		for (size_t m = 1; m < data.m(); ++m) {
			data(k, m) = cross_scheme(f, data, k-1, m);
		}
	}
	
}

void parallel_conv(const ftype &f, const utype &phi, const utype &psi, matrix_t &data, MPI_Comm *comm) {
	int id; MPI_Comm_rank(comm[0], &id);
	int np; MPI_Comm_size(comm[0], &np);
	double tau = data.tau(), h = data.h();
	
	int *displs  = (int *)malloc(np * sizeof(int));
	int *rcounts = (int *)malloc(np * sizeof(int));
	for (int i = 0; i < np; ++i) {
		uint64_t borders = get_borders(data.m(), i, np);
		displs [i] = LBMASK(borders) - 1;
		rcounts[i] = RBMASK(borders);
	}
	//printf("my id = %d| with borders (%ld, %d) and offset %d\n", id, data.n(), rcounts[id], displs[id]);
	matrix_t matrix(data.n(), rcounts[id], {0, tau*data.n()}, {0, h*rcounts[id]});
	if (id == 0) 
		for (size_t i = 0; i < data.n(); ++i)  matrix(i, 0) = psi(tau * i);
	for (int j = 0; j < rcounts[id]; ++j) matrix(0, j) = phi(h * (j + displs[id]));

	MPI_Status status;
	

	double prev_data, next_data;
	for (size_t k = 1; k < data.n(); ++k) {
		if (id != 0) {
			if (id + 1 != np) {
				MPI_Sendrecv(&matrix(k-1,             0), 1, MPI_DOUBLE, id-1, 0, &next_data, 1, MPI_DOUBLE, id+1, 0, comm[0], &status);
				MPI_Sendrecv(&matrix(k-1, rcounts[id]-1), 1, MPI_DOUBLE, id+1, 0, &prev_data, 1, MPI_DOUBLE, id-1, 0, comm[0], &status);
			} else {
					MPI_Sendrecv(&matrix(k-1,           0), 1, MPI_DOUBLE, id-1, 0, &prev_data, 1, MPI_DOUBLE, id-1, 0, comm[0], &status);
					next_data = matrix(k-1, rcounts[id]-1);
			}
		} else {	
			MPI_Sendrecv(&matrix(k-1, rcounts[id]-1), 1, MPI_DOUBLE, id+1, 0, &next_data, 1, MPI_DOUBLE, id+1, 0, comm[0], &status);
			prev_data = matrix(k-1, 0);
		}
		//if (id == 0) printf("id = %d| prev = %lg | next = %lg\n", id, prev_data, next_data);
		for (int m = ((id == 0) ? 1 : 0); m < rcounts[id]; ++m) 
			matrix(k, m) = cross_scheme(f, matrix, displs[id], k-1, m, {prev_data, next_data});
	}
	rcounts[0] = rcounts[0] * data.n(), displs[0] = 0;
	for (int i = 1; i < np; ++i) rcounts[i] *= data.n(), displs[i] = displs[i-1] + rcounts[i-1];
	//if (id == 1) matrix.dump(std::cout);

	int flag = 1;
	if (id == 0) {
		printf("id = %d\n", id);
		matrix.dump(std::cout);
		MPI_Send(&flag, 1, MPI_INT, id+1, 0, comm[0]);
	} else if (id + 1 == np) {
		MPI_Recv(&flag, 1, MPI_INT, id-1, 0, comm[0], &status);
		printf("id = %d\n", id);
		matrix.dump(std::cout);
	}
	else {
		MPI_Recv(&flag, 1, MPI_INT, id-1, 0, comm[0], &status);
		printf("id = %d\n", id);
		matrix.dump(std::cout);
		MPI_Send(&flag, 1, MPI_INT, id+1, 0, comm[0]);
	}
	MPI_Barrier(comm[0]);

	MPI_Gatherv(matrix.ptr(), rcounts[id], MPI_DOUBLE, data.ptr(), rcounts, displs, MPI_DOUBLE, 0, comm[0]);

	free(displs);
	free(rcounts);
}

