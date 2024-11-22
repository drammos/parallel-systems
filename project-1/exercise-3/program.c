//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"

typedef struct {
    long long int repetititons;
    int thread_id;
} ThreadData;

int thread_count;
int* shared_table;
void solution(long long int numberOfRepetitions, int numberOfThreads);

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
    shared_table = malloc(numberOfThreads*sizeof(int));
    for(int i = 0; i < numberOfThreads; i++)
    {
        shared_table[i] = 0;
    }
    solution(numberOfRepetitions, numberOfThreads);
}

void algorithm(void* input)
{
    ThreadData* thread = (ThreadData*)input;
    for(long long int i = 0; i < thread->repetititons; i++)
    {
        shared_table[i] += 1;
        // Please complet it
    }
}

void solution(long long int numberOfRepetitions, int numberOfThreads)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

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
        pthread_create(&thread_handles[t], NULL, (void*)algorithm, &thread_data[t]);
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