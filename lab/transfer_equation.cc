#include "transfer_equation.hh"

double cross_scheme(const ftype &f, const matrix_t &data, const size_t k, const size_t m) {
	size_t mn   = ((m == data.m() - 1) ? m : m+1);
	size_t kn_1 = ((k == 0) ? 0 : k-1 );
	double tmp = 2*data.tau()*(f(data.tau()*k, data.h()*m) - ((data(k,mn) - data(k,m-1)) / (2 * data.h()))) + data(kn_1, m);
	return tmp;
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

uint64_t get_borders(uint64_t last_num, int rank, int np) {
	uint32_t right_border = 1 + (last_num / np) * rank ;
	uint32_t left_border  = (rank == np - 1) ? last_num : (last_num / np) * (rank + 1);
		uint32_t temporary = left_border - right_border;
		temporary = ((temporary == 0) ? 0 : temporary + 1); 
	return ((uint64_t)right_border << 32) | (temporary); 
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
	double *matrix = (double *)malloc(data.n() * rcounts[id] * sizeof(double));
	if (id == 0) 
		for (int i = 0; i < data.n(); ++i)  matrix[i * rcounts[id]] = psi(tau * i);
	for (int j = 0; j < rcounts[id]; ++j) matrix[j] = phi(h * (j + displs[id]));

	MPI_Status status;
	double prev_data, next_data;
	for (int k = 1; k < data.n(); ++k) {
		if (id != 0) { 
			for (int i = 0; i < 2; ++i) {
				if ((id + i) % 2) { 
					MPI_Send(matrix+(k-1)*rcounts[id], 1, MPI_DOUBLE, id-1, 0, comm[0]); 
				} else {
					if (id + 1 != np)
						MPI_Recv(&next_data, 1, MPI_DOUBLE, id+1, 0, comm[0], &status); 
				}
			}
			MPI_Recv(&prev_data, 1, MPI_DOUBLE, id-1, 0, comm[0], &status);
		} else {
			prev_data = matrix[(k-1)*rcounts[id]];
			MPI_Recv(&next_data, 1, MPI_DOUBLE, id+1, 0, comm[0], &status);
		}
		std::cout << id << " | prev=" << prev_data << "| next=" << next_data << std::endl;
		for (int m = 0; m < rcounts[id]; ++m) {	
			if (m == 0) 
				matrix[k * rcounts[id] + m] = 2*tau*(f(tau*k, h*(m+displs[id])) - (matrix[k*rcounts[id] + m+1] - matrix[k*rcounts[id]+  m-1]) / (2*h)) + prev_data;
			else if (m + 1 == rcounts[id])
				matrix[k * rcounts[id] + m] = 2*tau*(f(tau*k, h*(m+displs[id])) - (next_data - matrix[k*rcounts[id]+  m-1])/(2*h)) + matrix[(k-1)*rcounts[id]+m];
			else 
				matrix[k * rcounts[id] + m] = 2*tau*(f(tau*k, h*(m+displs[id])) - (matrix[k*rcounts[id] + m+1] - matrix[k*rcounts[id]+  m-1]) / (2*h)) + matrix[(k-1)*rcounts[id]+m];
		}
		if (id != np - 1) {
			MPI_Send(matrix+k*rcounts[id]-1, 1, MPI_DOUBLE, id+1, 0, comm[0]);
		}
	}

	if (id == 0) {
		for (int j = 0; j < data.n(); ++j) {
			memcpy(data.ptr()+j*data.m(), matrix + rcounts[id]*j,rcounts[id] * sizeof(double));
		}
		int max_count = rcounts[0];
		for (int i = 0; i < np; ++i) max_count = ((max_count < rcounts[id]) ? rcounts[id] : max_count);
		double *buf = (double *)malloc(data.n() * max_count * sizeof(double));
		
		for (int i = 1; i < np; ++i) {
			MPI_Recv(buf, data.n() * rcounts[id], MPI_DOUBLE, i, 0, comm[0], &status);
			for (int j = 0; j < data.n(); ++j) {
				memcpy(data.ptr()+displs[i]+j*data.m(), buf + rcounts[i]*j,rcounts[i] * sizeof(double));
			}
		}

		free(buf);
	} else {
		MPI_Send(matrix, data.n() * rcounts[id], MPI_DOUBLE, 0, 0, comm[0]);
	}

	free(matrix);
}

