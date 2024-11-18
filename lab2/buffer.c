#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];
int head = 0, tail = 0, count = 0;

// semaphore and mutex declarations
pthread_mutex_t mutex;
sem_t empty;
sem_t full;

// initialize the buffer, mutex, and semaphores
void buffer_init() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
}

// insert item into the buffer
int insert_item(buffer_item item) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (count < BUFFER_SIZE) {
        buffer[head] = item;
        head = (head + 1) % BUFFER_SIZE;
        count++;
        printf("Producer produced %d\n", item);
    } else {
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        return -1;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return 0;
}

// remove item from the buffer
int remove_item(buffer_item *item) {
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    if (count > 0) {
        *item = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer consumed %d\n", *item);
    } else {
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        return -1;
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return 0;
}

// producer thread function
void *producer() {
    buffer_item item;
    unsigned int seed = time(NULL);
    while (1) {
        sleep(rand_r(&seed) % 4);
        item = rand_r(&seed);
        if (insert_item(item) < 0) {
            printf("Error: Producer failed to insert item\n");
        }
    }
    return NULL;
}

// consumer thread function
void *consumer() {
    buffer_item item;
    unsigned int seed = time(NULL);
    while (1) {
        sleep(rand_r(&seed) % 4);
        if (remove_item(&item) < 0) {
            printf("Error: Consumer failed to remove item\n");
        }
    }
    return NULL;
}

// destroy the buffer's mutex and semaphores
void buffer_destroy() {
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
}

