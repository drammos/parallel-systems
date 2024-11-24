#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"
#include "includes.h"

/* Struct for list nodes */
typedef struct {
    int reader_threads_reading;
    int reader_threads_waiting;
    int writer_thread_writing;
    int writers_threads_waiting;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    pthread_mutex_t count_mutex;
} State;

State* state;
/* Shared variables */
int         thread_count;


void* reader_work(void* rank);
void* writer_work(void* rank);
void Usage(char* prog_name);

int main(int argc, char* argv[]) {
    long i; 
    int key, success, attempts;
    pthread_t* readers_threads;
    pthread_t* writers_threads;

    int inserts_in_main;
    unsigned seed = 1;
    double start, finish;

    if (argc != 3) Usage(argv[0]);

    char *endptr1, *endptr2;
    errno = 0; 

    int readers = strtoll(argv[1], &endptr1, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr1 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr1);
        return 1;
    }

    int writers = strtoll(argv[2], &endptr2, 10);
    if (errno != 0) {
        perror("strtoll");
        return 1;
    }

    if (*endptr2 != '\0') {
        printf("Error: Non-numeric characters found: %s\n", endptr2);
        return 1;
    }

    thread_count = readers + writers;

    readers_threads = malloc(readers*sizeof(pthread_t));
    writers_threads = malloc(writers*sizeof(pthread_t));

    //Initialize struct state
    state = malloc(sizeof(State));
    state->reader_threads_reading = 0;
    state->reader_threads_waiting = 0;
    state->writer_thread_writing = 0;
    state->writers_threads_waiting = 0;
    pthread_cond_init(&state->readers_cond, NULL);
    pthread_cond_init(&state->writers_cond, NULL);

    pthread_mutex_init(&state->count_mutex, NULL);

    GET_TIME(start);
    for (i = 0; i < readers; i++)
    {
        pthread_create(&readers_threads[i], NULL, reader_work, (void*) i);
    }
    for (int j = 0; j < writers; j++)
    {
        pthread_create(&writers_threads[j], NULL, writer_work, (void*) j);
    }
        

    for (i = 0; i < readers; i++)
    {
        pthread_join(readers_threads[i], NULL);
    }
    for (i = 0; i < writers; i++)
    {
        pthread_join(writers_threads[i], NULL);
    }
      
   GET_TIME(finish);

   pthread_mutex_destroy(&state->count_mutex);
   free(readers_threads);
   free(writers_threads);
   free(state);

   return 0;
}  /* main */


void* reader_work(void* rank) {
    long my_rank = (long) rank;

    printf("I am reader with id: %d\n", my_rank);

    return NULL;
}

void* writer_work(void* rank) {
    long my_rank = (long) rank;

    printf("I am writer with id: %d\n", my_rank);

    return NULL;
}

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */