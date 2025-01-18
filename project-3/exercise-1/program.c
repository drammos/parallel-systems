//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"
#include <stdio.h>
#include <string.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(int argc, char* argv[])
{   
    char greeting[MAX_STRING];
    int my_rank;
    int num_procs;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    printf("Received from rank %d ---  %d\n", my_rank, num_procs);
    
    
    if (argc != 3) {
        if (my_rank == 0) printf("Usage: %s <num_generations> <grid_size>\n", argv[0]);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    char *endptr1, *endptr2;
    int states = strtoll(argv[1], &endptr1, 10);
    int grid_size = strtoll(argv[2], &endptr2, 10);

    // int rows_per_process = grid_size / num_procs;
    // int local_rows = rows_per_process + 2; 
    // int* local_grid = (int*)malloc(local_rows * grid_size * sizeof(int));


    if (my_rank != 0) {
        for (int i = 1; i < num_procs; i++) {
            sprintf(greeting, "Hello from rank %d of %d!", my_rank, num_procs);
            MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {
       // CODE
        for (int q = 1; q < num_procs; q++) {
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Received from rank %d: %s\n", q, greeting);
        }
       
    }

    MPI_Finalize();
    return 0;
    // MPI_algorithm(states, grid);

}



//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////