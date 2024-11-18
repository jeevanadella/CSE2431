#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define N 1200
#define M 1000
#define P 500

// Matrices
int A[N][M];
int B[M][P];
int C1[N][P]; // Result from single-threaded multiplication
int C[N][P];  // Result from multi-threaded multiplication

// Structure to hold data for each thread
typedef struct {
    int start_row; // Starting row for this thread
    int end_row;   // Ending row for this thread
    int (*A)[M];   // Pointer to matrix A
    int (*B)[P];   // Pointer to matrix B
    int (*C)[P];   // Pointer to result matrix C
} ThreadData;

// Initialize matrices A and B with some values
void initialize_matrices() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            A[i][j] = i + j; // Example initialization
        }
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            B[i][j] = j; // Example initialization
        }
    }
}

// Single-threaded matrix multiplication for baseline
void single_threaded_multiply() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            C1[i][j] = 0; // Initialize result cell
            for (int k = 0; k < M; k++) {
                C1[i][j] += A[i][k] * B[k][j]; // Multiply and accumulate
            }
        }
    }
}

// Thread function for multithreaded matrix multiplication
void* multiply_partial(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < P; j++) {
            data->C[i][j] = 0; // Initialize result cell
            for (int k = 0; k < M; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j]; // Multiply and accumulate
            }
        }
    }
    pthread_exit(NULL); // Exit the thread
}

// Multithreaded matrix multiplication
void multithreaded_multiply(int num_threads) {
    pthread_t threads[num_threads]; // Array to hold thread identifiers
    ThreadData thread_data[num_threads]; // Array to hold thread data
    int rows_per_thread = N / num_threads; // Calculate rows per thread

    // Create threads
    for (int t = 0; t < num_threads; t++) {
        thread_data[t].start_row = t * rows_per_thread; // Set start row
        thread_data[t].end_row = (t == num_threads - 1) ? N : (t + 1) * rows_per_thread; // Set end row
        thread_data[t].A = A; // Set matrix A
        thread_data[t].B = B; // Set matrix B
        thread_data[t].C = C; // Set result matrix C
        pthread_create(&threads[t], NULL, multiply_partial, &thread_data[t]); // Create thread
    }

    // Join threads
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL); // Wait for thread to finish
    }
}

// Function to compare matrices C1 and C element-wise
int check_error() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            if (C[i][j] != C1[i][j]) { // Compare elements
                return 1; // Error found
            }
        }
    }
    return 0; // No error
}

// Function to get execution time in seconds
double get_execution_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; // Calculate time difference
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]); // Print usage message
        return EXIT_FAILURE;
    }

    int max_threads = atoi(argv[1]); // Convert argument to integer
    if (max_threads < 1 || max_threads > 5) {
        fprintf(stderr, "Error: Number of threads should be an integer between 1 and 5.\n"); // Print error message
        return EXIT_FAILURE;
    }

    // Initialize matrices A and B
    initialize_matrices();

    // Perform single-threaded multiplication
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start); // Start timer
    single_threaded_multiply();
    clock_gettime(CLOCK_REALTIME, &end); // End timer
    double single_thread_time = get_execution_time(start, end); // Calculate execution time
    printf("Threads    Seconds    Error\n");
    printf("      1    %8.2f\n", single_thread_time);

    // Perform multithreaded multiplication with increasing thread counts
    for (int num_threads = 2; num_threads <= max_threads; num_threads++) {
        // Reset matrix C for each run
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < P; j++) {
                C[i][j] = 0; // Reset result cell
            }
        }

        // Measure multithreaded multiplication time
        clock_gettime(CLOCK_REALTIME, &start); // Start timer
        multithreaded_multiply(num_threads);
        clock_gettime(CLOCK_REALTIME, &end); // End timer
        double multi_thread_time = get_execution_time(start, end); // Calculate execution time

        // Check for errors
        int error = check_error();
        printf("      %d    %8.2f    %s\n", num_threads, multi_thread_time, error ? "Error" : "No error");
    }

    return EXIT_SUCCESS;
}
