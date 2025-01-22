#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


void allocate_matrix(double **matrix, int rows, int cols) {
    *matrix = (double *)malloc(rows * cols * sizeof(double));
}

void initialize_matrix(double *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i * cols + j] = (double)(rand() % 11);
        }
    }
}

void initialize_vector(double *vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = (double)(rand() % 11);
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


int main(int argc, char *argv[])
{
    
    int n;
    srand(time(NULL));


    int my_rank;
    int comm_sz;

    double *matrix = NULL;
    double *vector = NULL;

    double *result;


    double *local_matrix;
    double *local_result;

    double t_init_local, t_dist_local, t_comp_local, t_gather_local;

    double t_init_avg, t_dist_avg, t_comp_avg, t_gather_avg;
    double start_time, end_time;

    MPI_Datatype column_type, col;
    

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (argc != 2) {
        if (my_rank == 0) {
            printf("Usage: %s n\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    n = atoi(argv[1]);

    if(my_rank == 0) 
    {
        printf("Processes: %d\nMatrix:%dx%d\nVector: 1x%d\n\n", comm_sz, n, n, n);
    }

    
    if (n % comm_sz != 0) {
        if (my_rank == 0) {
            printf("Matrix size must be divisible by number of processes\n");
        }
        MPI_Finalize();
        return 1;
    }


    start_time = MPI_Wtime();

    int local_cols = n / comm_sz;
    int local_size = n * local_cols;

    allocate_matrix(&local_matrix, n, local_cols);
    allocate_matrix(&local_result, n, 1);
    vector = (double *)malloc(n * sizeof(double));

    if (my_rank == 0) {
        allocate_matrix(&matrix, n, n);
        initialize_matrix(matrix, n, n);
        initialize_vector(vector, n);
        
        // Print
        // printf("Initial data:\n");
        // print_matrix(matrix, n, n);
        // print_vector(vector, n, "Input Vector");
    }
    
    end_time = MPI_Wtime();
    t_init_local = end_time - start_time;


    start_time = MPI_Wtime();

    MPI_Type_vector(n, local_cols, n, MPI_DOUBLE, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, local_cols * sizeof(double), &column_type);
    MPI_Type_commit(&column_type);

    MPI_Scatter(matrix, 1, column_type, local_matrix, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Bcast(vector, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    t_dist_local = end_time - start_time ;
    // // If rank == 0
    // if (my_rank == 0) {
    //     // Send columns (local_matrix) to other proc
    //     for (int dest = 1; dest < comm_sz; dest++) {
    //         MPI_Send(&matrix[dest * local_cols], 1, column_type, dest, 0, MPI_COMM_WORLD);
    //     }

    //     // Create local_matrix for process = 0
    //     for (int i = 0; i < n; i++) {
    //         for (int j = 0; j < local_cols; j++) {
    //             local_matrix[i * local_cols + j] = matrix[i * n + j];
    //         }
    //     }
    // } 
    // else 
    // {
    //     // Receive columns from process 0 for the local_matrix
    //     MPI_Recv(local_matrix, local_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // }


    start_time = MPI_Wtime();

    for (int i = 0; i < n; i++) {
        local_result[i] = 0.0;
        for (int j = 0; j < local_cols; j++) {
            local_result[i] += local_matrix[i * local_cols + j] * vector[j + my_rank * local_cols];
        }
    }
    end_time = MPI_Wtime();
    t_comp_local = end_time - start_time;

    start_time = MPI_Wtime();

    if (my_rank == 0) {
        result = (double *)malloc(n * sizeof(double));
    }

    MPI_Reduce(local_result, result, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();
    t_gather_local = end_time - start_time;

    MPI_Reduce(&t_init_local, &t_init_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&t_dist_local, &t_dist_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&t_comp_local, &t_comp_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&t_gather_local, &t_gather_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        t_init_avg /= comm_sz;
        t_dist_avg /= comm_sz;
        t_comp_avg /= comm_sz;
        t_gather_avg /= comm_sz;

        // print_vector(result, n, "Result Vector");
        printf("Average Timing Results:\n");
        printf("Initialization time: %f seconds\n", t_init_avg);
        printf("Distribution time:   %f seconds\n", t_dist_avg);
        printf("Computation time:    %f seconds\n", t_comp_avg);
        printf("Gathering time:      %f seconds\n", t_gather_avg);
        printf("Total time:          %f seconds\n", 
               t_init_avg + t_dist_avg + t_comp_avg + t_gather_avg);
    }

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