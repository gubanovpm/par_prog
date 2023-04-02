#include "./sort.h"

int verify(const int *array, const size_t n, const int tos) {
    for (size_t i = 1; i < n; ++i) {
        if ((array[i] - array[i-1]) * tos < 0) {
            return 0;
        }
    }
    return 1;
}

void random_filling(int *array, const size_t n, const size_t max) {
    srand(time(NULL));
    for (size_t i = 0; i < n; ++i) {
        array[i] = rand() % max;
    }
}

void dump(const int *array, const size_t n) {
    for (size_t i = 0; i < n; ++i)
        printf("%d ", array[i]);
    printf("\n");
}

void parallel_odd_even_sort(MPI_Comm *comm, int *array, const size_t n, const int tos) {
    int id; MPI_Comm_rank(comm[0], &id);     // номер процесса 
    int np; MPI_Comm_size(comm[0], &np);     // количество процессов

    uint64_t borders = get_borders(n, id, np);
    int bg_n = LBMASK(borders) - 1, count_n = RBMASK(borders);
    int *a = (int *)calloc(count_n, sizeof(int));

    memcpy(a, array + bg_n, count_n * sizeof(int));
    straight_odd_even_sort(a, count_n, tos);
    // dump(a, count_n);
    MPI_Status status;

    for (int i = 0; i < np; i++) {
        if (i % 2 == 1) {
            if (id % 2 == 1) {
                if (id < np - 1) {
                    int len;
                    MPI_Recv(&len,   1, MPI_INT, id + 1, 0, comm[0], &status);
                    int *buf = (int *)malloc(len * sizeof(int));
                    MPI_Recv( buf, len, MPI_INT, id + 1, 0, comm[0], &status);
                    // MPI_Barrier(comm[0]);
                    split(a, count_n, buf, len, tos);
                    MPI_Send( buf, len, MPI_INT, id + 1, 0, comm[0]);
                    // MPI_Barrier(comm[0]);
                    free(buf);
                }
            } else if (id > 0) { 
                MPI_Send(&count_n, 1, MPI_INT, id - 1, 0, comm[0]);
                MPI_Send(a,  count_n, MPI_INT, id - 1, 0, comm[0]);
                // MPI_Barrier(comm[0]);
                MPI_Recv(a,  count_n, MPI_INT, id - 1, 0, comm[0], &status);
                // MPI_Barrier(comm[0]);
            }
        } else {
            if(id % 2 == 0) {
                if (id < np - 1) {
                    int len;
                    MPI_Recv(&len,   1, MPI_INT, id + 1, 0, comm[0], &status);
                    int *buf = (int *)malloc(len * sizeof(int));
                    MPI_Recv( buf, len, MPI_INT, id + 1, 0, comm[0], &status);
                    // MPI_Barrier(comm[0]);
                    split(a, count_n, buf, len, tos);
                    MPI_Send( buf, len, MPI_INT, id + 1, 0, comm[0]);
                    // MPI_Barrier(comm[0]);
                    free(buf);
                }
            } else {
                MPI_Send(&count_n, 1, MPI_INT, id - 1, 0, comm[0]);
                MPI_Send(a,  count_n, MPI_INT, id - 1, 0, comm[0]);
                // MPI_Barrier(comm[0]);
                MPI_Recv(a,  count_n, MPI_INT, id - 1, 0, comm[0], &status);
                // MPI_Barrier(comm[0]);
            }
        }
    }
    MPI_Barrier(comm[0]);
    // printf("%d part of array is: ", id); dump(a, count_n);
    MPI_Gather(a, count_n, MPI_INT, array + bg_n, count_n, MPI_INT, 0, comm[0]);
    free(a);
}

void straight_odd_even_sort(int *array, const size_t n, const int tos) {
    for (size_t i = 0; i < n; i++) {
        if (i % 2 == 1) {
            for (int j = 0; j < n/2 - (1 - n%2) ; j++) 
                cmp_swap(array + 2*j + 1, array + 2*j + 2, tos);
        } else {
            for (int j = 0; j < n/2 ; j++) 
                cmp_swap(array + 2*j, array + 2*j + 1, tos);
        }
    }
}

void cmp_swap(int *first, int *second, const int tos) {
    if ((first[0] - second[0]) * tos > 0) {
        int temporary = first[0];
        first[0]  = second[0];
        second[0] = temporary; 
    }
}

void swap(int *first, int *second) {
    int temporary = first[0];
    first[0]  = second[0];
    second[0] = temporary;
}

uint64_t get_borders(uint64_t last_num, int rank, int np) {
	uint32_t right_border = 1 + (last_num / np) * rank ;
	uint32_t left_border  = (rank == np - 1) ? last_num : (last_num / np) * (rank + 1);
    uint32_t temporary = left_border - right_border;
    temporary = ((temporary == 0) ? 0 : temporary + 1); 
	return ((uint64_t)right_border << 32) | (temporary); 
}

void split(int *a, const size_t n, int *b, const size_t m, const int tos) {
    int *temp = (int *)malloc((n+m) * sizeof(int));
    size_t i = 0, j = 0;
    while (i + j < n + m) {
        if ((b[j] - a[i]) * tos > 0)
            temp[i + j] = a[i], ++i;
        else
            temp[i + j] = b[j], ++j;
        if (i == n) for (; j < m; ++j) temp[i + j] = b[j];
        if (j == m) for (; i < n; ++i) temp[i + j] = a[i];
    }
    memcpy(a, temp + 0, n * sizeof(int));
    memcpy(b, temp + n, m * sizeof(int));
    free(temp);
}