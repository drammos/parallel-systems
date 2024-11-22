//////////////////////////////////////////////
//                                          //
//             MAIN                         //  
//                                          //
//////////////////////////////////////////////

#include "includes.h"

long long int mutex_global_price;
long long int atomic_global_price;
pthread_mutex_t mutex; 

typedef struct {
    long long int repetititons;
    int thread_id;
} ThreadData;

int thread_count;

void mutex_solution(long long int numberOfRepetititons, int numberOfThreads);
void atomic_solution(long long int numberOfRepetititons, int numberOfThreads);

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

    // Initialize global price
    mutex_global_price = 0;
    mutex_solution(numberOfRepetitions, numberOfThreads);

    atomic_global_price = 0;
    atomic_solution(numberOfRepetitions, numberOfThreads);
}

void algorithm_with_mutex(void* input)
{   
    ThreadData* thread = (ThreadData*)input;
    // printf("I am in %d thread from %d threads for rep: %lld\n", thread->thread_id, thread_count, thread->repetititons);
    for(long long int i = 0; i < thread->repetititons; i++)
    {
        pthread_mutex_lock(&mutex);
        mutex_global_price++;
        pthread_mutex_unlock(&mutex);
    }
}

void algorithm_with_atomic(void* input)
{
    ThreadData* thread = (ThreadData*)input;
    printf("I am in %d thread from %d threads for rep: %lld\n", thread->thread_id, thread_count, thread->repetititons);
    for(long long int i = 0; i < thread->repetititons; i++)
    {
        atomic_global_price++;
    }
}


void mutex_solution(long long int numberOfRepetititons, int numberOfThreads)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create the mutex
    pthread_mutex_init(&mutex, NULL);
    thread_count = numberOfThreads;
    pthread_t* thread_handles;
    ThreadData thread_data[thread_count];

    thread_handles = malloc(thread_count*sizeof(pthread_t));

    // Create pthreads
    for(int t = 0; t < thread_count; t++)
    {
        thread_data[t].thread_id = t;
        thread_data[t].repetititons = numberOfRepetititons;
        pthread_create(&thread_handles[t], NULL, (void*)algorithm_with_mutex, &thread_data[t]);
    }

    // Join all pthreads
    for(int t = 0; t < thread_count; t++)
    {
        pthread_join(thread_handles[t], NULL);
    }

    free(thread_handles);
    pthread_mutex_destroy(&mutex);

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("The global_price is: %lld\n", mutex_global_price);
    printf("The time for first way is: %f\n", elapsed_time);
}

void atomic_solution(long long int numberOfRepetititons, int numberOfThreads)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    thread_count = numberOfThreads;
    pthread_t* thread_handles;
    ThreadData thread_data[thread_count];

    thread_handles = malloc(thread_count*sizeof(pthread_t));

    for(int t = 0; t < thread_count; t++)
    {
        thread_data[t].thread_id = t;
        thread_data[t].repetititons = numberOfRepetititons;
        pthread_create(&thread_handles[t], NULL, (void*)algorithm_with_atomic, &thread_data[t]);
    }

    for(int t = 0; t < thread_count; t++)
    {
        pthread_join(thread_handles[t], NULL);
    }

    free(thread_handles);

    clock_gettime(CLOCK_MONOTONIC, &end); 
    double elapsed_time = (end.tv_sec - start.tv_sec) + 
                          (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("The global_price is: %lld\n", atomic_global_price);
    printf("The time for second way is: %f\n", elapsed_time);
}

//////////////////////////////////////////////
//                                          //
//                  END                     //  
//                                          //
//////////////////////////////////////////////