#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#include "Serial.h"
#include "includes.h"

#include <stdint.h>

double* row_algorithm(double** table_a, double* table_b, int n)
{
    printf("The n is:%d\n", n);
    double* table_x;
    table_x = (double*)malloc(n*sizeof(double));

    if (table_x == NULL) {
        fprintf(stderr, "Memory allocation failed for table_x\n");
        exit(EXIT_FAILURE);
    }

    for(int row = n-1; row >= 0; row--)
    {
        table_x[row] = table_b[row];
        for(int col = row+1; col < n; col++)
        {
            table_x[row] -= table_a[row][col]*table_x[col];
        }
        table_x[row] /= table_a[row][row];
    }
    return table_x;
}

double* column_algorithm(double** table_a, double* table_b, int n)
{
    double* table_x;
    table_x = malloc(n * sizeof(double));

    for(int row = 0; row < n; row++)
    {
        table_x[row] = table_b[row];
    }

    for(int col = n-1; col >= 0; col--)
    {
        table_x[col] /= table_a[col][col];
        for(int row = 0; row < col; row++)
        {
            table_x[row] -= table_a[row][col]*table_x[col];
        }
    }
    return table_x;
}

void serial_algorithm(double** table_a, double* table_b, int n, char* backtracking_algorithm){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    double* table_x;
    if(!strcmp("row", backtracking_algorithm))
    {
        table_x = row_algorithm(table_a, table_b, n);
    }
    else{
        table_x = column_algorithm(table_a, table_b, n);
    }

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    free(table_x);
    // printf("THE TABLE X:\n");
    // for(int i = 0; i < n; i++)
    // {
    //     if(i == n-1)
    //     {
    //         printf("x%d =  %d", i, table_x[i]);
    //     }
    //     else{
    //         printf("x%d =  %d, ", i, table_x[i]);
    //     }
        
    // }
    printf("\nThe time for Serial-Solution with %s algorithm is: %f\n\n", elapsed_time, backtracking_algorithm);
}