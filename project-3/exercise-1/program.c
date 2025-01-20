//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <Grid.h>

const int MAX_STRING = 100;
Cell** compute_local_state(Cell** local_grid, int rows, int cols, Cell* top_row, Cell* bottom_row) {
    printf("\n\n\nela\n\n\n");
    Cell** new_local_grid = initialize_grid(rows);
    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };

    printf("ela\n\n\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int neighbours_counter = 0;

            for (int d = 0; d < 8; d++) {
                int ni = i + directions[d][0];
                int nj = j + directions[d][1];

                // Handle boundary conditions including ghost rows
                if (nj >= 0 && nj < cols) {
                    if (ni == -1 && top_row != NULL) {
                        if (top_row[nj].state == LIVE) neighbours_counter++;
                    }
                    else if (ni == rows && bottom_row != NULL) {
                        if (bottom_row[nj].state == LIVE) neighbours_counter++;
                    }
                    else if (ni >= 0 && ni < rows) {
                        if (local_grid[ni][nj].state == LIVE) neighbours_counter++;
                    }
                }
            }

            // Apply Game of Life rules
            if (local_grid[i][j].state == LIVE && (neighbours_counter == 2 || neighbours_counter == 3)) {
                new_local_grid[i][j].state = LIVE;
            }
            else if (local_grid[i][j].state == DEAD && neighbours_counter == 3) {
                new_local_grid[i][j].state = LIVE;
            }
            else {
                new_local_grid[i][j].state = DEAD;
            }
        }
    }

    free_grid(local_grid, rows);
    return new_local_grid;
}

void parallel_algorithm(int states, int dimension) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Calculate local dimensions
    int rows_per_proc = dimension / size;
    int extra_rows = dimension % size;
    int local_rows = rows_per_proc + (rank < extra_rows ? 1 : 0);
    int start_row = rank * rows_per_proc + (rank < extra_rows ? rank : extra_rows);

    // Root process initializes the complete grid
    Cell** complete_grid = NULL;
    if (rank == 0) {
        complete_grid = create_grid(dimension);
    }

    // Allocate local grid for each process
    Cell** local_grid = initialize_grid(local_rows);
    
    // Create MPI datatype for Cell
    MPI_Datatype cell_type;
    MPI_Type_contiguous(1, MPI_INT, &cell_type);
    MPI_Type_commit(&cell_type);
    // Arrays to store counts and displacements for scattering
    int* sendcounts = malloc(size * sizeof(int));
    int* displs = malloc(size * sizeof(int));
    // Calculate send counts and    displacements
    for (int i = 0; i < size; i++) {
        sendcounts[i] = (rows_per_proc + (i < extra_rows ? 1 : 0)) * dimension;
        displs[i] = (i * rows_per_proc + (i < extra_rows ? i : extra_rows)) * dimension;
    }

    // Main time evolution loop
    struct timespec start, end;
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
    }
    for (int state = 0; state < states; state++) {
        // Scatter the grid
        
        if (rank == 0) {
            for (int i = 0; i < size; i++) {
                if (i == 0) {
                    
             printf("\n\n\nela %d local_rows = %d, dimension = %d\n\n\n", rank, local_rows, dimension);
                    for (int r = 0; r < local_rows; r++) {
                        for (int c = 0; c < dimension; c++) {
                            local_grid[r][c] = complete_grid[r][c];
                        }
                    }
                    
             printf("\n\n\nela %d\n\n\n", rank);
                } else {
                    int dest_rows = rows_per_proc + (i < extra_rows ? 1 : 0);
                    for (int r = 0; r < dest_rows; r++) {
                        MPI_Send(&complete_grid[displs[i]/dimension + r][0], dimension, cell_type, 
                                i, 0, MPI_COMM_WORLD);
                    }
                }
            }
        } else {
             printf("\n\n\nela %d\n\n\n", rank);
            for (int r = 0; r < local_rows; r++) {
                MPI_Recv(&local_grid[r][0], dimension, cell_type, 0, 0, 
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        // Exchange boundary rows
        Cell *top_row = NULL, *bottom_row = NULL;
        if (rank > 0) {
            top_row = malloc(dimension * sizeof(Cell));
            MPI_Send(&local_grid[0][0], dimension, cell_type, rank-1, 1, MPI_COMM_WORLD);
            MPI_Recv(top_row, dimension, cell_type, rank-1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size-1) {
            bottom_row = malloc(dimension * sizeof(Cell));
            MPI_Recv(bottom_row, dimension, cell_type, rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&local_grid[local_rows-1][0], dimension, cell_type, rank+1, 2, MPI_COMM_WORLD);
        }

        // Compute new state
        local_grid = compute_local_state(local_grid, local_rows, dimension, top_row, bottom_row);

        // Free boundary rows
        free(top_row);
        free(bottom_row);

        // Gather results back to root
        if (rank == 0) {
            for (int r = 0; r < local_rows; r++) {
                for (int c = 0; c < dimension; c++) {
                    complete_grid[r][c] = local_grid[r][c];
                }
            }
            for (int i = 1; i < size; i++) {
                int source_rows = rows_per_proc + (i < extra_rows ? 1 : 0);
                for (int r = 0; r < source_rows; r++) {
                    MPI_Recv(&complete_grid[displs[i]/dimension + r][0], dimension, cell_type, 
                            i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
        } else {
            for (int r = 0; r < local_rows; r++) {
                MPI_Send(&local_grid[r][0], dimension, cell_type, 0, 3, MPI_COMM_WORLD);
            }
        }
    }
    
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("The time for Parallel-Solution with %d processes is: %f\n\n", size, elapsed_time);
        // print_grid(complete_grid, dimension);
        free_grid(complete_grid, dimension);
    }

    // Cleanup
    free_grid(local_grid, local_rows);
    free(sendcounts);
    free(displs);
    MPI_Type_free(&cell_type);
}




int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    if (argc != 3) {
        printf("Usage: %s <dimension> <states>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int dimension = atoi(argv[1]);
    int states = atoi(argv[2]);
    
    parallel_algorithm(states, dimension);
    
    MPI_Finalize();
    return 0;
}


// int main(int argc, char* argv[])
// {   
//     char greeting[MAX_STRING];
//     int my_rank;
//     int num_procs;


//     MPI_Init(&argc, &argv);
//     MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
//     MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
//     printf("Received from rank %d ---  %d\n", my_rank, num_procs);
    
    
//     if (argc != 3) {
//         if (my_rank == 0) printf("Usage: %s <num_generations> <grid_size>\n", argv[0]);
//         MPI_Finalize();
//         return EXIT_FAILURE;
//     }

//     char *endptr1, *endptr2;
//     int states = strtoll(argv[1], &endptr1, 10);
//     int grid_size = strtoll(argv[2], &endptr2, 10);

//     // int rows_per_process = grid_size / num_procs;
//     // int local_rows = rows_per_process + 2; 
//     // int* local_grid = (int*)malloc(local_rows * grid_size * sizeof(int));


//     if (my_rank != 0) {
//         for (int i = 1; i < num_procs; i++) {
//             sprintf(greeting, "Hello from rank %d of %d!", my_rank, num_procs);
//             MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
//         }
//     } else {
//        // CODE
//         for (int q = 1; q < num_procs; q++) {
//             MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//             printf("Received from rank %d: %s\n", q, greeting);
//         }
       
//     }

//     MPI_Finalize();
//     return 0;
//     // MPI_algorithm(states, grid);

// }



//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////