#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LIVE 1
#define DEAD 0

typedef struct {
    int state;
} Cell;

// Function Prototypes
Cell** initialize_grid(int rows, int cols);
void free_grid(Cell** grid, int rows);
Cell** read_file_rle(Cell** grid, int dimension);
void print_grid(Cell** grid, int dimension);
Cell** create_grid(int dimension);
void parallel_algorithm(int states, int dimension);


int main(int argc, char** argv) {
    int dimension = 10;   // Example grid dimension
    int states = 5;     // Number of iterations

    parallel_algorithm(states, dimension);

    return 0;
}


// Parallel Game of Life algorithm using MPI_Scatterv
void parallel_algorithm(int states, int dimension) {
    int rank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Determine rows per process and handle uneven division
    int rows_per_process = dimension / size;
    int extra_rows = dimension % size;
    int local_rows = rows_per_process + (rank < extra_rows ? 1 : 0);
    
    int* sendcounts = (int*)malloc(size * sizeof(int));
    int* displs = (int*)malloc(size * sizeof(int));
    
    if (!sendcounts || !displs) {
        fprintf(stderr, "Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Calculate send counts and displacements
    int total_rows = 0;
    for (int i = 0; i < size; i++) {
        sendcounts[i] = (rows_per_process + (i < extra_rows ? 1 : 0)) * dimension;
        displs[i] = total_rows;
        total_rows += sendcounts[i];
    }

    // Allocate local grid with ghost rows
    Cell** local_grid = initialize_grid(local_rows + 2, dimension);
    if (!local_grid) {
        free(sendcounts);
        free(displs);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Root process initializes and distributes the grid
    Cell** full_grid = NULL;
    if (rank == 0) {
        full_grid = create_grid(dimension);
        if (!full_grid) {
            free_grid(local_grid, local_rows + 2);
            free(sendcounts);
            free(displs);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Scatter the grid data
    for (int i = 0; i < local_rows; i++) {
        MPI_Scatterv(
            rank == 0 ? &(full_grid[0][0].state) : NULL,
            sendcounts,
            displs,
            MPI_INT,
            &(local_grid[i + 1][0].state),
            sendcounts[rank],
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
    }

    // Free full grid after scattering
    if (rank == 0) {
        free_grid(full_grid, dimension);
        full_grid = NULL;
    }

    struct timespec start_time, end_time;
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }

    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},          {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (int state = 0; state < states; state++) {
        // Exchange ghost rows
        if (rank % 2 != 0) {
            // Even-ranked processes
            // Send up, then receive from up
            if (rank > 0) {
                MPI_Send(&(local_grid[1][0].state), dimension, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&(local_grid[0][0].state), dimension, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            // Send down, then receive from down
            if (rank < size - 1) {
                MPI_Send(&(local_grid[local_rows][0].state), dimension, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
                MPI_Recv(&(local_grid[local_rows + 1][0].state), dimension, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        else {
            // Odd-ranked processes
            // Receive from up, then send up
            if (rank > 0) {
                MPI_Recv(&(local_grid[0][0].state), dimension, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&(local_grid[1][0].state), dimension, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
            }
            // Receive from down, then send down
            if (rank < size - 1) {
                MPI_Recv(&(local_grid[local_rows + 1][0].state), dimension, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&(local_grid[local_rows][0].state), dimension, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
            }
        }

        // Allocate new grid for next state
        Cell** new_local_grid = initialize_grid(local_rows + 2, dimension);
        if (!new_local_grid) {
            free_grid(local_grid, local_rows + 2);
            free(sendcounts);
            free(displs);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Initialize new grid to DEAD state
        for (int i = 0; i < local_rows + 2; i++) {
            for (int j = 0; j < dimension; j++) {
                new_local_grid[i][j].state = DEAD;
            }
        }

        // Compute new state
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 0; j < dimension; j++) {
                int neighbors = 0;
                for (int d = 0; d < 8; d++) {
                    int ni = i + directions[d][0];
                    int nj = j + directions[d][1];

                    if (ni >= 0 && ni <= local_rows + 1 && nj >= 0 && nj < dimension) {
                        neighbors += local_grid[ni][nj].state == LIVE;
                    }
                }

                // Apply Game of Life rules
                if (local_grid[i][j].state == LIVE && (neighbors == 2 || neighbors == 3)) {
                    new_local_grid[i][j].state = LIVE;
                } else if (local_grid[i][j].state == DEAD && neighbors == 3) {
                    new_local_grid[i][j].state = LIVE;
                }
            }
        }

        // Free old grid and update pointer
        free_grid(local_grid, local_rows + 2);
        local_grid = new_local_grid;
    }

    // Gather final results
    Cell** final_grid = NULL;
    if (rank == 0) {
        final_grid = initialize_grid(dimension, dimension);
        if (!final_grid) {
            free_grid(local_grid, local_rows + 2);
            free(sendcounts);
            free(displs);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Gather results
    if (rank == 0) {
        // Copy local results
        for (int i = 0; i < local_rows; i++) {
            memcpy(&(final_grid[i][0].state), &(local_grid[i + 1][0].state), dimension * sizeof(Cell));
        }

        // Receive from other processes
        for (int p = 1; p < size; p++) {
            int proc_rows = rows_per_process + (p < extra_rows ? 1 : 0);
            int start_row = displs[p] / dimension;
            
            for (int i = 0; i < proc_rows; i++) {
                MPI_Recv(&(final_grid[start_row + i][0].state), dimension, MPI_INT, 
                        p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                            (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        printf("The time for Parallel-Solution is: %f seconds\n", elapsed_time);

        print_grid(final_grid, dimension);
        free_grid(final_grid, dimension);
    } else {
        // Send local results to root
        for (int i = 1; i <= local_rows; i++) {
            MPI_Send(&(local_grid[i][0].state), dimension, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    // Cleanup
    free_grid(local_grid, local_rows + 2);
    free(sendcounts);
    free(displs);

    MPI_Finalize();
}