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
    int dimension = 20;   // Example grid dimension
    int states = 100;     // Number of iterations

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

    int* sendcounts = (int*)malloc(size * sizeof(int));
    int* displs = (int*)malloc(size * sizeof(int));
    int total_rows = 0;

    for (int i = 0; i < size; i++) {
        sendcounts[i] = (rows_per_process + (i < extra_rows ? 1 : 0)) * dimension;
        displs[i] = total_rows;
        total_rows += sendcounts[i];
    }

    // Local grid size (with ghost rows)
    int local_rows = rows_per_process + (rank < extra_rows ? 1 : 0);
    Cell** local_grid = initialize_grid(local_rows + 2, dimension);

    // Root initializes the full grid
    Cell** full_grid = NULL;
    if (rank == 0) {
        full_grid = create_grid(dimension);
    }

    // Scatter the grid rows
    for (int i = 0; i < local_rows; i++) {
        MPI_Scatterv(
            rank == 0 ? full_grid[0] : NULL, // Send buffer
            sendcounts,                      // Send counts
            displs,                          // Displacements
            MPI_INT,                         // Data type
            local_grid[i + 1],               // Receive buffer (skip ghost rows)
            sendcounts[rank],                // Receive count
            MPI_INT,                         // Data type
            0,                               // Root
            MPI_COMM_WORLD                   // Communicator
        );
    }

    if (rank == 0) {
        free_grid(full_grid, dimension);
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
        if (rank > 0) {
            MPI_Send(local_grid[1], dimension, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(local_grid[0], dimension, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Send(local_grid[local_rows], dimension, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(local_grid[local_rows + 1], dimension, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Compute new state
        Cell** new_local_grid = initialize_grid(local_rows + 2, dimension);

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

        free_grid(local_grid, local_rows + 2);
        local_grid = new_local_grid;
    }

    // Gather results at the root
    if (rank == 0) {
        Cell** full_grid = initialize_grid(dimension, dimension);

        for (int i = 0; i < local_rows; i++) {
            memcpy(full_grid[i], local_grid[i + 1], dimension * sizeof(Cell));
        }

        for (int p = 1; p < size; p++) {
            for (int i = 0; i < sendcounts[p] / dimension; i++) {
                MPI_Recv(full_grid[displs[p] / dimension + i], dimension, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                              (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        printf("The time for Parallel-Solution is: %f seconds\n", elapsed_time);

        print_grid(full_grid, dimension);
        free_grid(full_grid, dimension);
    } else {
        for (int i = 1; i <= local_rows; i++) {
            MPI_Send(local_grid[i], dimension, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    free(sendcounts);
    free(displs);
    free_grid(local_grid, local_rows + 2);
    if (rank == 0)
    {
        free_grid(full_grid, dimension);
    }
    MPI_Finalize();
}
