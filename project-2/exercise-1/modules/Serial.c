
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include "Serial.h"
#include "includes.h"
#include "Grid.h"

#include <stdint.h>

Cell** create_new_state(Cell** grid, int dimension);

void serial_algorithm(int states, int dimension)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    Cell** grid = create_grid(dimension);
    // print_grid(grid, dimension);

    // Next states
    for(int i = 1; i < states; i++){
        grid = create_new_state(grid, dimension);
        // print_grid(grid, dimension);
    }

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The time for Serial-Solution is: %f\n\n", elapsed_time);
}


Cell** create_new_state(Cell** grid, int dimension)
{
    Cell** new_grid_state = initialize_grid(dimension);

    for(int i = 0; i < dimension; i++)
    {
        for(int j = 0; j < dimension; j++)
        {
            int neighbours_counter = 0;

            // For neighbours above the line
            if(i > 0)
            {
                if(j > 0)
                {
                    if(grid[i-1][j-1].state == LIVE) neighbours_counter++;
                }
                
                if(grid[i-1][j].state == LIVE) neighbours_counter++;

                if(j + 1 != dimension)
                {
                    if(grid[i-1][j+1].state == LIVE) neighbours_counter++;
                }
            }

            if(j>0)
            {
                if(grid[i][j-1].state == LIVE) neighbours_counter++; 
            }

            if(j+1 != dimension)
            {
                if(grid[i][j+1].state == LIVE) neighbours_counter++; 
            }

            // For neighbours under the line
            if( i+1 != dimension)
            {
                if(j > 0)
                {
                    if(grid[i+1][j-1].state == LIVE) neighbours_counter++;
                }
                
                if(grid[i+1][j].state == LIVE) neighbours_counter++;

                if(j + 1 != dimension)
                {
                    if(grid[i+1][j+1].state == LIVE) neighbours_counter++;
                }
            }

            // Calculate it for new_grid_state
            if(grid[i][j].state == LIVE && (neighbours_counter==2 || neighbours_counter==3))
            {
                new_grid_state[i][j].state = LIVE;
            }
            else if( grid[i][j].state == DEAD && neighbours_counter == 3)
            {
                new_grid_state[i][j].state = LIVE;
            }
        }
    }

    free_grid(grid, dimension);
    return new_grid_state;
}