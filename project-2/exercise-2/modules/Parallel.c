#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#include "Parallel.h"
#include "includes.h"

#include <stdint.h>

void parellel_algorithm(int** table_a, int* table_b, int n, char* backtracking_algorithm, int threads){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);



    
    

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The time for Parallel-Solution with %s algorithm is: %f\n\n", elapsed_time, backtracking_algorithm);
}