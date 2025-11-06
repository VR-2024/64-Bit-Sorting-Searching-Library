#include "ultra_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memcmp, memcpy
#include <time.h>   // For time
#include <stdint.h> // For int64_t

// Helper function to print a small array (for debugging)
void print_small_array(const char* title, int64_t* array, size_t count) {
    printf("  %-10s [", title);
    for (size_t i = 0; i < count; i++) {
        // Use %lld for long long, which is standard for 64-bit integers
        printf("%lld", (long long)array[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
}

// Comparison function for C's qsort (must be 64-bit aware)
int qsort_compare(const void* a, const void* b) {
    int64_t val_a = *(const int64_t*)a;
    int64_t val_b = *(const int64_t*)b;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

// Helper function to check if the sort is correct
int check_sort(const char* test_name, int64_t* array_to_sort, int64_t* expected_array, size_t count) {
    // Run our assembly language sort
    ultra_sort_radix(array_to_sort, count);

    // Compare the result
    if (memcmp(array_to_sort, expected_array, count * sizeof(int64_t)) == 0) {
        printf("[PASS]: %s\n", test_name);
        return 1;
    } else {
        printf("[FAIL]: %s\n", test_name);
        // Only print arrays if they are small
        if (count < 20) {
            print_small_array("Got", array_to_sort, count);
            print_small_array("Expected", expected_array, count);
        }
        return 0;
    }
}

int main() {
    printf("--- Testing Radix Sort (radix_sort_asm) ---\n");
    int pass_count = 0;
    int total_tests = 0;

    // Test 1: Small Reverse Sorted
    total_tests++;
    int64_t reverse_array[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    int64_t reverse_expected[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    pass_count += check_sort("Small Reverse Sorted", reverse_array, reverse_expected, 11);

    // Test 2: Small Already Sorted
    total_tests++;
    int64_t sorted_array[] = {100, 200, 300, 400, 500};
    int64_t sorted_expected[] = {100, 200, 300, 400, 500};
    pass_count += check_sort("Small Already Sorted", sorted_array, sorted_expected, 5);
    
    // Test 3: All Duplicates
    total_tests++;
    int64_t duplicate_array[] = {7, 7, 7, 7, 7};
    int64_t duplicate_expected[] = {7, 7, 7, 7, 7};
    pass_count += check_sort("All Duplicates", duplicate_array, duplicate_expected, 5);

    // Test 4: Negative Numbers (This is the one that failed before)
    total_tests++;
    int64_t negative_array[] = {0, -1, -5, -10, -2};
    int64_t negative_expected[] = {-10, -5, -2, -1, 0};
    pass_count += check_sort("Negative Numbers", negative_array, negative_expected, 5);

    // Test 5: Large Random Array (with positive and negative numbers)
    total_tests++;
    printf("Generating 100000 random 64-bit integers...\n");
    const size_t LARGE_SIZE = 100000;
    int64_t* large_test_array = (int64_t*)malloc(LARGE_SIZE * sizeof(int64_t));
    int64_t* large_expected_array = (int64_t*)malloc(LARGE_SIZE * sizeof(int64_t));

    if (!large_test_array || !large_expected_array) {
        printf("[FATAL]: Failed to allocate memory for large test.\n");
        return 1;
    }

    srand(time(NULL));
    for (size_t i = 0; i < LARGE_SIZE; i++) {
        // --- NEW: Generate true 64-bit signed numbers ---
        uint64_t high = ((uint64_t)rand() << 32);
        uint64_t low = (uint64_t)rand();
        uint64_t rand_64 = high | low;
        large_test_array[i] = (int64_t)rand_64; // Cast to signed
    }

    // Copy to the expected array *before* sorting
    memcpy(large_expected_array, large_test_array, LARGE_SIZE * sizeof(int64_t));

    // Sort the expected array with C's qsort
    printf("Sorting 100000 elements with C qsort (for comparison)...\n");
    qsort(large_expected_array, LARGE_SIZE, sizeof(int64_t), qsort_compare);

    // Sort the test array with our assembly code
    printf("Sorting 100000 elements with ultra_sort_radix...\n");
    pass_count += check_sort("Large Random Array (100K)", large_test_array, large_expected_array, LARGE_SIZE);

    free(large_test_array);
    free(large_expected_array);

    printf("--- Summary: %d / %d tests passed ---\n", pass_count, total_tests);

    return (pass_count == total_tests) ? 0 : 1;
}