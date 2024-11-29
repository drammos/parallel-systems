//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 

typedef struct {
    long long int repetititons;
    int thread_id;
} ThreadData;

int thread_count;
void solution(long long int numberOfRepetitions, int numberOfThreads);
pthread_barrier_t barrier;

int main(int argc, char* argv[])
{   
    if( argc != 3)
    {
        printf("WRONG INPUT FROM ARGS!!!\n");
        exit(EXIT_FAILURE);
    }

    char *endptr1, *endptr2;
    errno = 0; 

    long long int numberOfRepetitions = strtoll(argv[1], &endptr1, 10);
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


    printf("Number Of Repetitions: %lld\nNumberOfThreads: %d\n", numberOfRepetitions, numberOfThreads);
    solution(numberOfRepetitions, numberOfThreads);
}

void Thread_Work(void* input)
{
    ThreadData* thread = (ThreadData*)input;
    
    for(long long int i = 0; i < thread->repetititons; i++)
    {
        pthread_barrier_wait(&barrier);
    }
}

void solution(long long int numberOfRepetitions, int numberOfThreads)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (pthread_barrier_init(&barrier, NULL, numberOfThreads) != 0) {
        perror("Barrier initialization failed");
        exit(EXIT_FAILURE);
    }

    // Create the mutex
    thread_count = numberOfThreads;
    pthread_t* thread_handles;
    ThreadData thread_data[thread_count];

    thread_handles = malloc(thread_count*sizeof(pthread_t));

    // Create pthreads
    for(int t = 0; t < thread_count; t++)
    {
        thread_data[t].thread_id = t;
        thread_data[t].repetititons = numberOfRepetitions;
        pthread_create(&thread_handles[t], NULL, (void*)Thread_Work, &thread_data[t]);
    }

    // Join all pthreads
    for(int t = 0; t < thread_count; t++)
    {
        pthread_join(thread_handles[t], NULL);
    }

    free(thread_handles);

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("The time for Solution is: %f\n\n", elapsed_time);
}

//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////