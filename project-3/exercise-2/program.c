#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void allocate_matrix(double **matrix, int rows, int cols) {
    *matrix = (double *)malloc(rows * cols * sizeof(double));
}

void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i * cols + j] = (double)(i + j);
        }
    }
}

void initialize_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = 2.0;  // Changed to 1.0 for easier verification
    }
}

void print_matrix(double *matrix, int rows, int cols) {
    printf("Matrix (%dx%d):\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%6.2f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_vector(double *vector, int size, const char *name) {
    printf("%s (%d):\n", name, size);
    for (int i = 0; i < size; i++) {
        printf("%6.2f ", vector[i]);
    }
    printf("\n\n");
}

void print_local_matrix(double *local_matrix, int rows, int local_cols, int rank) {
    printf("Process %d Local Matrix (%dx%d):\n", rank, rows, local_cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < local_cols; j++) {
            printf("%6.2f ", local_matrix[i * local_cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void gather_and_print_final_matrix(double *local_matrix, int n, int local_cols, int my_rank, int comm_sz) {
    double *gathered_matrix = NULL;
    if (my_rank == 0) {
        gathered_matrix = (double *)malloc(n * n * sizeof(double));
    }

    // Gather all local matrices to process 0
    for (int i = 0; i < n; i++) {
        MPI_Gather(&local_matrix[i * local_cols], local_cols, MPI_DOUBLE,
                  gathered_matrix ? &gathered_matrix[i * n] : NULL, local_cols, 
                  MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Print the gathered matrix in process 0
    if (my_rank == 0) {
        printf("Final Distributed Matrix State:\n");
        print_matrix(gathered_matrix, n, n);
        free(gathered_matrix);
    }
}

int main(int argc, char *argv[]) {
    int n;                  // Matrix size
    int my_rank;           // Process rank
    int comm_sz;           // Number of processes
    double *matrix = NULL; // Full matrix (only in process 0)
    double *vector = NULL; // Input vector
    double *local_matrix;  // Local portion of matrix
    double *result;        // Result vector
    double *local_result;  // Local result
    MPI_Datatype column_type; // Custom MPI type for column distribution

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Get matrix size from command line
    if (argc != 2) {
        if (my_rank == 0) {
            printf("Usage: %s n\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    n = atoi(argv[1]);

    // Verify if n is divisible by number of processes
    if (n % comm_sz != 0) {
        if (my_rank == 0) {
            printf("Matrix size must be divisible by number of processes\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Calculate local sizes
    int local_cols = n / comm_sz;
    int local_size = n * local_cols;

    // Allocate memory for local data
    allocate_matrix(&local_matrix, n, local_cols);
    allocate_matrix(&local_result, n, 1);
    vector = (double *)malloc(n * sizeof(double));

    // Process 0 initializes the full matrix and vector
    if (my_rank == 0) {
        allocate_matrix(&matrix, n, n);
        initialize_matrix(matrix, n, n);
        initialize_vector(vector, n);
        
        // Print initial matrix and vector
        printf("Initial data:\n");
        print_matrix(matrix, n, n);
        print_vector(vector, n, "Input Vector");
    }
    
    // Create MPI vector type for distributing columns
    MPI_Type_vector(n, local_cols, n, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);

    // Broadcast vector to all processes
    MPI_Bcast(vector, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Distribute matrix columns
    if (my_rank == 0) {
        // Send columns to other processes
        for (int dest = 1; dest < comm_sz; dest++) {
            MPI_Send(&matrix[dest * local_cols], 1, column_type, dest, 0, MPI_COMM_WORLD);
        }
        // Copy local columns for process 0
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < local_cols; j++) {
                local_matrix[i * local_cols + j] = matrix[i * n + j];
            }
        }
    } else {
        // Receive columns from process 0
        MPI_Recv(local_matrix, local_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Print local matrices for verification
    MPI_Barrier(MPI_COMM_WORLD);  // Synchronize output
    for (int i = 0; i < comm_sz; i++) {
        if (my_rank == i) {
            print_local_matrix(local_matrix, n, local_cols, my_rank);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Gather and print the final distributed matrix state
    gather_and_print_final_matrix(local_matrix, n, local_cols, my_rank, comm_sz);

    // Perform local matrix-vector multiplication
    for (int i = 0; i < n; i++) {
        local_result[i] = 0.0;
        for (int j = 0; j < local_cols; j++) {
            local_result[i] += local_matrix[i * local_cols + j] * vector[j + my_rank * local_cols];
        }
    }

    // Allocate memory for final result in process 0
    if (my_rank == 0) {
        result = (double *)malloc(n * sizeof(double));
    }

    // Reduce local results to get final result
    MPI_Reduce(local_result, result, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print result in process 0
    if (my_rank == 0) {
        print_vector(result, n, "Result Vector");
    }

    // Clean up
    MPI_Type_free(&column_type);
    free(vector);
    free(local_matrix);
    free(local_result);
    if (my_rank == 0) {
        free(matrix);
        free(result);
    }

    MPI_Finalize();
    return 0;
}