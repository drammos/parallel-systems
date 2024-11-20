//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"

int main(int argc, char* argv[])
{   
    printf("In main Function\n");

    if( argc != 3)
    {
        printf("WRONG INPUT FROM ARGS!!!\n");
        exit(EXIT_FAILURE);
    }

    char *endptr1, *endptr2;
    errno = 0; 

    long long int numberOfCastings = strtoll(argv[1], &endptr1, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    int numberOfThreads = strtoll(argv[2], &endptr2, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr2 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr2);
        return 1;
    }


    printf("Number Of Castings: %lld\nNumberOfThreads: %d\n", numberOfCastings, numberOfThreads);
    serialAlgo(numberOfCastings);
    parallelAlgo(numberOfCastings, numberOfThreads);
}


void serialAlgo(long long int numberOfCastings)
{   
    clock_t start, end;
    start = clock();
    srand(time(NULL));
    long long int arrow_circle = 0;
    for(long long int arrow = 0; arrow < numberOfCastings; arrow++)
    {
        double x = ((double)rand() / RAND_MAX) * 2 - 1;
        double y = ((double)rand() / RAND_MAX) * 2 - 1;
        double distanse_square = x*x + y*y;
        if( distanse_square <=1 ){
            arrow_circle++;
        }
    }
    
    double p = 4*arrow_circle/(numberOfCastings);
    end = clock();

    // Prints
    printf("the Arrow circle is: %d\n", arrow_circle);
    printf("the p is: %.15f\n", p);
    printf("The time for Serial algorithm is: %f\n", ((double) (end - start)) / CLOCKS_PER_SEC);
}


void parallelAlgo(long long int numberOfCastings, int numberOfThreads)
{
    clock_t start, end;
    start = clock();
    srand(time(NULL));

    //Code for parallel

    //

    end = clock();
    printf("The time for Parallel algorithm is: %f\n", ((double) (end - start)) / CLOCKS_PER_SEC);
}

//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////