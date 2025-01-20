
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdbool.h>
// #include <ctype.h>
// #include <stdlib.h>
// #include <limits.h>

// #include "MPI.h"
// #include "includes.h"
// #include "Grid.h"

// #include <stdint.h>

// Cell** create_new_state(Cell** grid, int dimension);

// void MPI_algorithm(int states, int dimension)
// {
//     struct timespec start, end;
//     clock_gettime(CLOCK_MONOTONIC, &start);
//     Cell** grid = create_grid(dimension);
//     // print_grid(grid, dimension);

//     // Next states
//     for(int i = 1; i < states; i++){
//         grid = create_new_state(grid, dimension);
//         // print_grid(grid, dimension);
//     }

//     clock_gettime(CLOCK_MONOTONIC, &end); 
//     double elapsed_time = (end.tv_sec - start.tv_sec) + 
//                           (end.tv_nsec - start.tv_nsec) / 1e9;

//     printf("The time for MPI-Solution is: %f\n\n", elapsed_time);
// }


// Cell** create_new_state(Cell** grid, int dimension)
// {
//     Cell** new_grid_state = initialize_grid(dimension);

//     int directions[8][2] = {
//         {-1, -1}, {-1, 0}, {-1, 1},
//         {0, -1},          {0, 1},
//         {1, -1}, {1, 0}, {1, 1}
//     };

//     for (int i = 0; i < dimension; i++) {
//         for (int j = 0; j < dimension; j++) {
//             int neighbours_counter = 0;

//             for (int d = 0; d < 8; d++) {
//                 int ni = i + directions[d][0];
//                 int nj = j + directions[d][1];

//                 if (ni >= 0 && ni < dimension && nj >= 0 && nj < dimension) {
//                     if (grid[ni][nj].state == LIVE) {
//                         neighbours_counter++;
//                     }
//                 }
//             }


//             if(grid[i][j].state == LIVE && (neighbours_counter==2 || neighbours_counter==3))
//             {
//                 new_grid_state[i][j].state = LIVE;
//             }
//             else if( grid[i][j].state == DEAD && neighbours_counter == 3)
//             {
//                 new_grid_state[i][j].state = LIVE;
//             }
//         }
//     }

//     free_grid(grid, dimension);
//     return new_grid_state;
// }