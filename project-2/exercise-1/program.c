//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"
#include <string.h>
#include "Serial.h"

int main(int argc, char* argv[])
{   
    if( argc != 5)
    {
        printf("WRONG INPUT FROM ARGS!!!\n");
        exit(EXIT_FAILURE);
    }

    char *endptr1, *endptr2;
    errno = 0; 

    int states = strtoll(argv[1], &endptr1, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    int grid = strtoll(argv[2], &endptr1, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    char* algorithm = argv[3]; 

    int numberOfThreads = strtoll(argv[4], &endptr2, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr2 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr2);
        return 1;
    }


    printf("states: %d\nGrid: %dx%d\nAlgorithm: %s\nNumberOfThreads: %d\n", states, grid, grid,algorithm, numberOfThreads);
    
    if(!strcmp("serial", algorithm))
    {
        serial_algorithm(states, grid);
    }
    else
    {
        // parallel_algorithm(states, grid, numberOfThreads);
    }
}



//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////