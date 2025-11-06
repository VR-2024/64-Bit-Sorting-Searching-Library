// FIX: Define POSIX source to get CLOCK_MONOTONIC
// This MUST be before any #include
#define _POSIX_C_SOURCE 199309L

#include "ultra_sort.h" // Our library
#include <stdio.h>
#include <stdlib.h> // For qsort, malloc, free, rand
#include <string.h> // For memcpy, memcmp
#include <time.h>   // For clock_gettime
#include <stdint.h> // For int64_t

// Use 1 Million elements, as specified in the documentation's performance target
#define ARRAY_SIZE 1000000

// High-resolution timer function
double get_time_ms() {
    struct timespec ts;
    // This line will now be valid thanks to the #define above
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

// Comparison function for C's qsort
int qsort_compare(const void* a, const void* b) {
    int64_t val_a = *(const int64_t*)a;
    int64_t val_b = *(const int64_t*)b;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

int main() {
    printf("--- Phase 8: Benchmarking (1 Million 64-bit Integers) ---\n");

    // --- 1. Allocate Memory ---
    int64_t* original_data = (int64_t*)malloc(ARRAY_SIZE * sizeof(int64_t));
    int64_t* qsort_copy = (int64_t*)malloc(ARRAY_SIZE * sizeof(int64_t));
    int64_t* ultra_sort_copy = (int64_t*)malloc(ARRAY_SIZE * sizeof(int64_t));

    if (!original_data || !qsort_copy || !ultra_sort_copy) {
        printf("[FATAL]: Memory allocation failed!\n");
        return 1;
    }

    // --- 2. Generate Random 64-bit Signed Data ---
    printf("Generating %d random 64-bit numbers...\n", ARRAY_SIZE);
    srand(time(NULL));
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        uint64_t high = ((uint64_t)rand() << 32);
        uint64_t low = (uint64_t)rand();
        original_data[i] = (int64_t)(high | low);
    }

    // Copy data to our two test arrays
    memcpy(qsort_copy, original_data, ARRAY_SIZE * sizeof(int64_t));
    memcpy(ultra_sort_copy, original_data, ARRAY_SIZE * sizeof(int64_t));
    printf("Data generation complete.\n\n");

    // --- 3. Benchmark Standard C qsort ---
    printf("Running standard C qsort...\n");
    double start_time_qsort = get_time_ms();
    
    qsort(qsort_copy, ARRAY_SIZE, sizeof(int64_t), qsort_compare);
    
    double end_time_qsort = get_time_ms();
    double time_qsort = end_time_qsort - start_time_qsort;
    printf("  qsort time: %.2f ms\n\n", time_qsort);

    // --- 4. Benchmark Our ultra_sort ---
    printf("Running ultra_sort (assembly)...\n");
    double start_time_ultra = get_time_ms();
    
    ultra_sort(ultra_sort_copy, ARRAY_SIZE); // Call our main adaptive sort
    
    double end_time_ultra = get_time_ms();
    double time_ultra = end_time_ultra - start_time_ultra;
    printf("  ultra_sort time: %.2f ms\n\n", time_ultra);

    // --- 5. Verify Correctness and Print Results ---
    printf("--- Results ---\n");
    if (memcmp(qsort_copy, ultra_sort_copy, ARRAY_SIZE * sizeof(int64_t)) != 0) {
        printf("[FAIL]: ultra_sort produced an incorrect result!\n");
    } else {
        printf("[PASS]: Sort is correct.\n");
        printf("  Standard qsort: %.2f ms\n", time_qsort);
        printf("  Assembly ultra_sort: %.2f ms\n", time_ultra);
        printf("  Speedup: %.2fx\n", time_qsort / time_ultra);
    }

    // --- 6. Clean up ---
    free(original_data);
    free(qsort_copy);
    free(ultra_sort_copy);

    return 0;
}