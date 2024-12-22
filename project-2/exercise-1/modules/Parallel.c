
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include "Parallel.h"
#include "includes.h"
#include "Grid.h"

#include <stdint.h>
void parellel_algorithm(int states, int dimension)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    Cell** grid = create_grid(dimension);
    

    // Next states
    

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The time for Parallel-Solution is: %f\n\n", elapsed_time);
}