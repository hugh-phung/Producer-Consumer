/* *************************************************************************************************************
 * Name: Hugh Phung
 * Student number: 3842508
 * Year: 2021
 * Project: The Producer-Consumer Problem


 * Method:
 * Create an array of size 10 to hold items produced
 * Have five threads that are concurrently producing items to fill the buckets
 * Have five threads that are concurrently consuming those items - printing out to screen
 * Have the program run for 10 seconds and then clearn up and exit without crashes, race conditions or deadlocks

 * Limitations:
 * Would use pthread_cond_broadcast if possible
 * *************************************************************************************************************
 */ 

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define NUM_THREADS 5
#define MAX_ITEMS 10
#define TIME_DURATION 10
#define EMPTY 0

#define ALPHABET 26
#define INITIAL_CHAR 'A'


char buckets[MAX_ITEMS];
pthread_mutex_t mutexBucket;
pthread_cond_t condFilled;
pthread_cond_t condEmptied;
int count = 0;
bool stop = false;


// Implements a first-in-last-out algorithm since the array is called a bucket
void *produceItems(void* var)
{
    // Uses a stop boolean to run the thread until stop is changed to true in main
    while (!stop)
    {
        // Locks thread since it is in the process of filling up the bucket - critical section
        pthread_mutex_lock(&mutexBucket);

        // If the bucket is full, wait until a signal from the consumer
        while (count >= MAX_ITEMS)
        {
            printf("Buckets Full, Number of items in Bucket: %d \n", count);
            pthread_cond_wait(&condEmptied, &mutexBucket);

        }

        // Producing random characters
        int num = rand() % ALPHABET;
        char item = INITIAL_CHAR + num;
        
        // Assign item produced to the buckets array
        buckets[count] = item;

        printf("Produced item: %c, Number of items in Bucket: %d \n", item, count+1);

        ++count;

        // Unlocks threads and signals the consumer to start consuming
        pthread_mutex_unlock(&mutexBucket);
        pthread_cond_signal(&condFilled);

        // If permitted, would rather use pthread_cond_broadcast to wake up all waiting threads
        // pthread_cond_broadcast(&condBucket);
    }

    return EXIT_SUCCESS;
}

void *consumeItems(void* var)
{
    // Uses a stop boolean to run the thread until stop is changed to true in main
    while (!stop)
    {
        // Locks mutex to prevent anything else from being produced whilst this thread is running - critical section
        pthread_mutex_lock(&mutexBucket);
        while (count <= EMPTY)
        {
            // If the array is empty then it waits for a signal from the producer to fill it back up
            printf("No items, currently waiting... Number of items in Bucket: %d \n", count);
            pthread_cond_wait(&condFilled, &mutexBucket);
        }

        // Deprecating count here because count[10] would be out of bounds for an array of length 10
        --count;

        // Consumed item output
        printf("Consumed item: %c. Items left: %d \n", buckets[count], count);

        // Setting erased item to null character
        buckets[count] = '\0';

        // Unlocks mutex and signals the producer to continue producing
        pthread_mutex_unlock(&mutexBucket);       
        pthread_cond_signal(&condEmptied);
    }

    return EXIT_SUCCESS;
}

int main (void)
{
    // Randomizer
    srand(time(NULL));

    // Create 5 threads for generating items for bucket
    pthread_t threadItemProduce[NUM_THREADS];
    // Create 5 threads that are constantly consuming items in the bucket
    pthread_t threadItemConsume[NUM_THREADS];

    // Initialising mutex and conditionals
    pthread_mutex_init(&mutexBucket, NULL);
    pthread_cond_init(&condEmptied, NULL);
    pthread_cond_init(&condFilled, NULL);
    int i;

    // Creates 5 producer and 5 consumer threads
    // Each concurrently produce and consume items from the buckets array for 10 seconds
    // Program will stop after 10 seconds by passing through a boolean argument
    printf("Start \n");
    for (i = 0; i < NUM_THREADS; ++i)
    {
        if (pthread_create(&threadItemProduce[i], NULL, &produceItems, NULL) != 0)
        {
            fprintf(stderr, "Failed to create a producer thread");
        }
    }

    for (i = 0; i < NUM_THREADS; ++i)
    {
        if (pthread_create(&threadItemConsume[i], NULL, &consumeItems, NULL) != 0)
        {
            fprintf(stderr, "Failed to create a consumer thread");
        }
    }

    // Let the program run for 10 seconds before stopping
    sleep(TIME_DURATION);
    stop = true;

    // Joining threads to ensure that they have properly terminated 
    for (i = 0; i < NUM_THREADS; ++i)
    {
        if (pthread_join(threadItemProduce[i], NULL) != 0)
        {
            fprintf(stderr, "Failed to join producer thread");
        }
    }

    for (i = 0; i < NUM_THREADS; ++i)
    {
        if (pthread_join(threadItemConsume[i], NULL) != 0)
        {
            fprintf(stderr, "Failed to join consumer thread");
        }
    }

    // Cleaning up mutex and conditions
    pthread_mutex_destroy(&mutexBucket);
    pthread_cond_destroy(&condEmptied);
    pthread_cond_destroy(&condFilled);

    printf("\nCompleted after %d seconds.\n", TIME_DURATION);

    return EXIT_SUCCESS;
}
