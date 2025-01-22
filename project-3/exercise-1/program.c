#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIVE 1
#define DEAD 0

#define IDX(i, j, width) ((i) * (width) + (j))

void initialize_grid(int* grid, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        grid[i] = (rand() % 2 == 0) ? DEAD : LIVE;
    }
}

void print_grid(int* grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", grid[IDX(i, j, cols)] == LIVE ? '#' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char** argv) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int states = atoi(argv[1]);
    int dimension = atoi(argv[2]);
    if(rank == 0) 
    {
        printf("Processes: %d\nGenerations: %d\nGrid:%dx%d\n\n", size,states, dimension, dimension);
    }
    double start_time = MPI_Wtime();
    int local_rows = dimension / size;
    if (rank < dimension % size) local_rows++;
    
    int* rows_per_process = NULL;
    int* displacement = NULL;
    int* sendcounts = NULL;
    int* displs = NULL;
    
    if (rank == 0) {
        rows_per_process = (int*)malloc(size * sizeof(int));
        displacement = (int*)malloc(size * sizeof(int));
        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        
        int offset = 0;
        for (int i = 0; i < size; i++) {
            rows_per_process[i] = dimension / size;
            if (i < dimension % size) rows_per_process[i]++;
            sendcounts[i] = rows_per_process[i] * dimension;
            displs[i] = offset * dimension;
            displacement[i] = offset;
            offset += rows_per_process[i];
        }
    }
    
    int grid_size = (local_rows + 2) * dimension;
    int* local_grid = (int*)malloc(grid_size * sizeof(int));
    int* new_local_grid = (int*)malloc(grid_size * sizeof(int));
    
    int* full_grid = NULL;
    if (rank == 0) {
        full_grid = (int*)malloc(dimension * dimension * sizeof(int));
        initialize_grid(full_grid, dimension, dimension);
    }
    
    MPI_Scatterv(full_grid, sendcounts, displs, MPI_INT,
                 local_grid + dimension, local_rows * dimension, MPI_INT,
                 0, MPI_COMM_WORLD);
    
    for (int iter = 0; iter < states; iter++) {
        MPI_Request requests[4];
        MPI_Status statuses[4];
        int req_count = 0;

        if (rank > 0) {
            MPI_Irecv(local_grid, dimension, MPI_INT, 
                      rank - 1, 0, MPI_COMM_WORLD, &requests[req_count++]);
        }
        
        if (rank < size - 1) {
            MPI_Irecv(local_grid + (local_rows + 1) * dimension, dimension, MPI_INT,
                      rank + 1, 1, MPI_COMM_WORLD, &requests[req_count++]);
        }

        if (rank > 0) {
            MPI_Isend(local_grid + dimension, dimension, MPI_INT,
                      rank - 1, 1, MPI_COMM_WORLD, &requests[req_count++]);
        }
        
        if (rank < size - 1) {
            MPI_Isend(local_grid + local_rows * dimension, dimension, MPI_INT,
                      rank + 1, 0, MPI_COMM_WORLD, &requests[req_count++]);
        }

        MPI_Waitall(req_count, requests, statuses);
        
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 0; j < dimension; j++) {
                int neighbors = 0;
                
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (di == 0 && dj == 0) continue;
                        
                        int ni = i + di;
                        int nj = j + dj;
                        
                        if (ni >= 0 && ni <= local_rows + 1 && 
                            nj >= 0 && nj < dimension) {
                            neighbors += local_grid[IDX(ni, nj, dimension)] == LIVE;
                        }
                    }
                }
                
                int curr_state = local_grid[IDX(i, j, dimension)];
                new_local_grid[IDX(i, j, dimension)] = DEAD;  
                
                if (curr_state == LIVE) {
                    if (neighbors == 2 || neighbors == 3) {
                        new_local_grid[IDX(i, j, dimension)] = LIVE;
                    }
                } else if (neighbors == 3) {
                    new_local_grid[IDX(i, j, dimension)] = LIVE;
                }
            }
        }
        
        int* temp = local_grid;
        local_grid = new_local_grid;
        new_local_grid = temp;

        MPI_Gatherv(local_grid + dimension, local_rows * dimension, MPI_INT,
                    full_grid, sendcounts, displs, MPI_INT,
                    0, MPI_COMM_WORLD);
    }
    
    free(local_grid);
    free(new_local_grid);
    
    if (rank == 0) {
        free(full_grid);
        free(rows_per_process);
        free(displacement);
        free(sendcounts);
        free(displs);
        double end_time = MPI_Wtime();
   
        printf("Elapsed time: %f seconds\n", end_time - start_time);
    }
    
    MPI_Finalize();
    return 0;
}