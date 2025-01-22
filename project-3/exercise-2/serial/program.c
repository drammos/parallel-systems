#include <stdio.h>
#include <stdlib.h>

#include <time.h>

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


int main(int argc, char* argv[])
{   
    if (argc != 2) {
        printf("Usage: %s n\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Full matrix and Vector
    double *matrix = NULL;
    double *vector = NULL;
    double *result;
    result = (double *)malloc(n * sizeof(double));
    vector = (double *)malloc(n * sizeof(double));


    // Allocate and initialization
    allocate_matrix(&matrix, n, n);
    initialize_matrix(matrix, n, n);
    initialize_vector(vector, n);


    // Print Matrix and Vector
    printf("Initial data:\n");
    print_matrix(matrix, n, n);
    print_vector(vector, n, "Input Vector");


    // Computation for serial
    for(int i = 0; i < n; i++){
        result[i] = 0.0;
        for(int j = 0; j < n; j++)
        {
            result[i] += matrix[i*n + j] * vector[j];
        }
    }    

    print_vector(result, n, "Result Vector");

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The time for Serial-Solution is: %f\n\n", elapsed_time);
    return 0;
}