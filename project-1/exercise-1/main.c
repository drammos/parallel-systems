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
    printf("The time for Serial algorithm is: %f\n\n\n", ((double) (end - start)) / CLOCKS_PER_SEC);
}
typedef struct {
    long long int start;
    long long int end;
    int thread_id;
} ThreadData;

int thread_count;
long long int total_arrow_circle = 0;

void Algo(void* input)
{
    ThreadData* thread = (ThreadData*)input;
    // printf("I am in %d thread from %d threads\n", thread->thread_id, thread_count);
    // printf("for thread: %d, start: %d, end: %d\n", thread->thread_id, thread->start, thread->end);
    long long int arrow_circle = 0;
    unsigned int seed = thread->start;
    for( long long int arrow = thread->start; arrow< thread->end; arrow++)
    {
        double x = ((double)rand_r(&seed) / RAND_MAX)* 2 - 1;
        double y = ((double)rand_r(&seed) / RAND_MAX) * 2 - 1;
        double distanse_square = x*x + y*y;
        if( distanse_square <=1 ){
            arrow_circle++;
        }
    }

    total_arrow_circle += arrow_circle;
}





void parallelAlgo(long long int numberOfCastings, int numberOfThreads)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    //Code for parallel
    long thread;
    pthread_t* thread_handles;
    ThreadData thread_data[numberOfThreads];

    thread_count = numberOfThreads;
    thread_handles = malloc(thread_count*sizeof(pthread_t));

    long long int chunk_size = numberOfCastings/(long long int)numberOfThreads;
    long long int remainder = numberOfCastings % (long long int)numberOfThreads;

    printf("Chunk size is: %lld and remainder is: %lld\n", chunk_size, remainder);



    //Create pthreads
    for(int t = 0; t < thread_count; t++)
    {
        thread_data[t].start = t*chunk_size;
        thread_data[t].end = (t+1)*chunk_size;
        if(t == numberOfThreads-1){
            thread_data[t].end += remainder;
        }
        thread_data[t].thread_id = t;
        pthread_create(&thread_handles[t], NULL, Algo, &thread_data[t]);
    }

    //Join with pthreads
    for(int t = 0; t < thread_count; t++)
    {
        pthread_join(thread_handles[t], NULL);
    }

    double p = 4*total_arrow_circle/(numberOfCastings);
    free(thread_handles);

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
                          
    printf("the Arrow circle is: %lld\n", total_arrow_circle);
    printf("the p is: %.15f\n", p);
    printf("The time for Parallel algorithm is: %f\n", elapsed_time);
}

//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////