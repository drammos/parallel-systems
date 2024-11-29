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


int thread_count;
long long int number_of_repetitions;
int counter;
void solution();

typedef struct{
    int thread_barrier_number;
    pthread_mutex_t lock;
    int total_thread;
} Thread_Barrier;

Thread_Barrier barrier;



int main(int argc, char* argv[])
{   
    if( argc != 3)
    {
        printf("WRONG INPUT FROM ARGS!!!\n");
        exit(EXIT_FAILURE);
    }

    char *endptr1, *endptr2;
    errno = 0; 

    number_of_repetitions = strtoll(argv[1], &endptr1, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    thread_count = strtoll(argv[2], &endptr2, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr2 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr2);
        return 1;
    }


    printf("Number Of Repetitions: %lld\nNumberOfThreads: %d\n", number_of_repetitions, thread_count);
    solution();
}

void thread_barrier_init(Thread_Barrier *barrier)
{
    pthread_mutex_init(&(barrier->lock), NULL);
    barrier->total_thread = 0;
}

void thread_barrier_destroy(Thread_Barrier *barrier){
    pthread_mutex_destroy(&(barrier->lock));
}


void thread_barrier_wait(Thread_Barrier *barrier){
    if(!pthread_mutex_lock(&(barrier->lock))){
        barrier->total_thread += 1;
        pthread_mutex_unlock(&(barrier->lock));
    }

    while (barrier->total_thread < thread_count);
    
    if(!pthread_mutex_lock(&(barrier->lock))){
        barrier->total_thread -= 1;
        pthread_mutex_unlock(&(barrier->lock));
    }
}

void Thread_Work(void* input)
{   
    thread_barrier_wait(&barrier);
}

void solution()
{   
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    thread_barrier_init(&barrier);

    pthread_t* thread_handles;

    thread_handles = malloc(thread_count*sizeof(pthread_t));

    // Create pthreads
    for(int t = 0; t < thread_count; t++)
    {
        pthread_create(&thread_handles[t], NULL, (void*)Thread_Work, t);
    }

    // Join all pthreads
    for(int t = 0; t < thread_count; t++)
    {
        pthread_join(thread_handles[t], NULL);
    }

    thread_barrier_destroy(&barrier);
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