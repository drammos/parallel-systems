//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initialize_matrix(int* matrix, int n) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = rand() % 11;
        }
    }
}

void initialize_vector(int* vector, int n) {
    srand(time(NULL) + 1);
    for (int i = 0; i < n; i++) {
        vector[i] = rand() % 11;
    }
}

void print_matrix(int* matrix, int n) {
    printf("\nMatrix is:\n\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i * n + j]);
        }
        printf("\n");
    }
}

void print_vector(int* vector, int n) {
    printf("\nVector is:\n\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

// int main(int argc, char* argv[])
// {   
//     int rank, size;
    
//     MPI_Init(&argc, &argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     int n = atoi(argv[1]);
//     double start_time = MPI_Wtime();

//     int* matrix = NULL;
//     int* vector = NULL;

//     // Initialize Grid
//     if(rank == 0)
//     {
//         matrix = (int*)malloc(n * n * sizeof(int));
//         initialize_matrix(matrix, n);
//         vector = (int*)malloc(n * sizeof(int));
//         initialize_vector(vector, n);

//         print_matrix(matrix, n);
//         print_vector(vector, n);
//     }






//     if (rank == 0) 
//     {
//         free(matrix);
//         free(vector);
//         double end_time = MPI_Wtime();
//         printf("Elapsed time: %f seconds\n", end_time - start_time);
//     }

//     MPI_Finalize();
//     return 0;
// }

int main(int argc, char* argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) {
            printf("Usage: %s <matrix_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);
    double start_time = MPI_Wtime();

    int* matrix = NULL;      // Full matrix (used only on rank 0)
    int* vector = NULL;      // Full vector
    int* local_matrix = NULL; // Submatrix for each process
    int* local_result = NULL; // Local result vector
    int* result = NULL;       // Final result vector on rank 0

    // Number of columns per process (block-column distribution)
    int cols_per_proc = n / size;
    int remainder = n % size;
    int local_cols = cols_per_proc + (rank < remainder ? 1 : 0);
    int* sendcounts = NULL;
    int* displs = NULL;

    if (rank == 0) {
        matrix = (int*)malloc(n * n * sizeof(int));
        vector = (int*)malloc(n * sizeof(int));
        result = (int*)malloc(n * sizeof(int));
        initialize_matrix(matrix, n);
        initialize_vector(vector, n);

        // Print the matrix and vector
        print_matrix(matrix, n);
        print_vector(vector, n);

        // Prepare sendcounts and displacements for scattering
        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        int offset = 0;

        for (int i = 0; i < size; i++) {
            int cols = cols_per_proc + (i < remainder ? 1 : 0);
            sendcounts[i] = cols * n; // Each process receives cols * n elements
            displs[i] = offset * n;
            offset += cols;
        }
    }

    local_matrix = (int*)malloc(local_cols * n * sizeof(int));
    local_result = (int*)calloc(n, sizeof(int)); // Initialize to 0

    MPI_Scatterv(matrix, sendcounts, displs, MPI_INT,
                 local_matrix, local_cols * n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        MPI_Bcast(vector, n, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        vector = (int*)malloc(n * sizeof(int));
        MPI_Bcast(vector, n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    for (int i = 0; i < local_cols; i++) {
        for (int j = 0; j < n; j++) {
            local_result[j] += local_matrix[i * n + j] * vector[i + rank * cols_per_proc + (rank < remainder ? rank : remainder)];
        }
    }

    // Gather the results to the root process
    MPI_Reduce(local_result, result, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print the result on rank 0
    if (rank == 0) {
        printf("\nResulting vector:\n");
        print_vector(result, n);

        // Free memory on rank 0
        free(matrix);
        free(vector);
        free(result);
        free(sendcounts);
        free(displs);
    }

    // Free local memory
    free(local_matrix);
    free(local_result);
    if (rank != 0) free(vector);

    double end_time = MPI_Wtime();
    if (rank == 0) {
        printf("\nElapsed time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}


//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////