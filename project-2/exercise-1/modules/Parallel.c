
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
#include "Grid.h"

#include <stdint.h>


Cell** create_new_state_parallel(Cell** grid, int dimension, int threads)
{
    Cell** new_grid_state = initialize_grid(dimension);

    #pragma omp parallel for num_threads(threads) collapse(2)
    for(int i = 0; i < dimension; i++)
    {
        for( int j = 0; j < dimension; j++)
        {
            int neighbours_counter = 0;

            // For neighbors above the current cell
            if (i > 0) {
                if (j > 0 && grid[i-1][j-1].state == LIVE) neighbours_counter++;
                if (grid[i-1][j].state == LIVE) neighbours_counter++;
                if (j + 1 != dimension && grid[i-1][j+1].state == LIVE) neighbours_counter++;
            }

            // For neighbors to the left and right of the current cell
            if (j > 0 && grid[i][j-1].state == LIVE) neighbours_counter++; 
            if (j + 1 != dimension && grid[i][j+1].state == LIVE) neighbours_counter++; 

            // For neighbors below the current cell
            if (i + 1 != dimension) {
                if (j > 0 && grid[i+1][j-1].state == LIVE) neighbours_counter++;
                if (grid[i+1][j].state == LIVE) neighbours_counter++;
                if (j + 1 != dimension && grid[i+1][j+1].state == LIVE) neighbours_counter++;
            }

            // Apply the Game of Life rules
            if (grid[i][j].state == LIVE && (neighbours_counter == 2 || neighbours_counter == 3)) {
                new_grid_state[i][j].state = LIVE;
            } else if (grid[i][j].state == DEAD && neighbours_counter == 3) {
                new_grid_state[i][j].state = LIVE;
            }
        }
    }

    free_grid(grid, dimension);
    return new_grid_state;
}

void parellel_algorithm(int states, int dimension, int threads)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    omp_set_num_threads(threads);
    printf("threads is: %d\n", threads);

    Cell** grid = create_grid(dimension);
    

    // Next states
    for(int i = 1; i < states; i++)
    {
        grid = create_new_state_parallel(grid, dimension, threads);
        // printf("The i= %d\n", i);
        // print_grid(grid, dimension);
    }
    

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The time for Parallel-Solution is: %f\n\n", elapsed_time);
}

