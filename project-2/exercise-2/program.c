//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "includes.h"
#include "Serial.h"
#include "Parallel.h"

int main(int argc, char* argv[])
{   
    if( argc != 5)
    {
        printf("WRONG INPUT FROM ARGS!!!\n");
        exit(EXIT_FAILURE);
    }

    char *endptr1, *endptr2;
    errno = 0; 

    int n = strtoll(argv[1], &endptr1, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    char* algorithm = argv[2];
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    char* backtracking_algorithm = argv[3];
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    int numberOfThreads = strtoll(argv[4], &endptr2, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr2 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr2);
        return 1;
    }


    printf("n: %d\nAlgorithm: %s\nBacktracking Algorithm: %s\nNumberOfThreads: %d\n", n, algorithm, backtracking_algorithm, numberOfThreads);

    double** table_a;
    double* table_b;

    table_a = malloc(n*sizeof(double*));
    table_b = malloc(n*sizeof(double));
    
    for(int i = 0; i < n; i++)
    {
        table_a[i] = malloc(n*sizeof(double));
    }


    srand(NULL);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++)
        {
            if(j>=i)
            {
                table_a[i][j] = rand() % 10 + 1;
            }
            else{
                table_a[i][j] = 0.0;
            }
            
        }
        table_b[i] = rand() % 100;
    }
    
    if(!strcmp("serial", algorithm))
    {
        serial_algorithm(table_a, table_b, n, backtracking_algorithm);
    }
    else
    {
        parellel_algorithm(table_a, table_b, n, backtracking_algorithm, numberOfThreads);
    }

    free(table_b);
    for(int i = 0; i < n; i++)
    {
        free(table_a[i]);
    }
    free(table_a);
}



//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////