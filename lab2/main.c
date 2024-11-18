#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

extern void buffer_init();
extern int insert_item(int item);
extern int remove_item(int *item);
extern void buffer_destroy();
extern void *producer(void *param);
extern void *consumer(void *param);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <sleep time> <producer threads> <consumer threads>\n", argv[0]);
        return -1;
    }

    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    // validate inputs
    if (sleep_time <= 0 || sleep_time > 10 || num_producers <= 0 || num_producers > 5 || num_consumers <= 0 || num_consumers > 5) {
        fprintf(stderr, "Error: Invalid arguments\n");
        return -1;
    }

    // initialize the buffer
    buffer_init();

    // create producer and consumer threads
    pthread_t producers[num_producers], consumers[num_consumers];

    for (int i = 0; i < num_producers; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // sleep for the user-specified time
    sleep(sleep_time);

    // clean up and exit
    buffer_destroy();

    return 0;
}

