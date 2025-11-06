#include "ultra_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memcmp, memcpy
#include <time.h>   // For time
#include <stdint.h> // For int64_t

// Comparison function for C's qsort (must be 64-bit aware)
int qsort_compare(const void* a, const void* b) {
    int64_t val_a = *(const int64_t*)a;
    int64_t val_b = *(const int64_t*)b;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

// Helper function to check if the sort is correct
// This function calls the MAIN ultra_sort function
int check_sort(const char* test_name, int64_t* array_to_sort, int64_t* expected_array, size_t count) {
    
    // --- Call the MAIN Sorter ---
    // This will internally call the algorithm selector.
    ultra_sort(array_to_sort, count);

    // Compare the result
    if (memcmp(array_to_sort, expected_array, count * sizeof(int64_t)) == 0) {
        printf("[PASS]: %s\n", test_name);
        return 1;
    } else {
        printf("[FAIL]: %s\n", test_name);
        // Only print arrays if they are small
        if (count < 20) {
            printf("  Got      [");
            for(size_t i=0; i<count; i++) printf("%lld, ", (long long)array_to_sort[i]);
            printf("]\n");
            
            printf("  Expected [");
            for(size_t i=0; i<count; i++) printf("%lld, ", (long long)expected_array[i]);
            printf("]\n");
        }
        return 0;
    }
}

// Helper to create a new test array and its "expected" copy
int64_t* create_array(int64_t* template_arr, size_t count) {
    int64_t* new_arr = (int64_t*)malloc(count * sizeof(int64_t));
    if (new_arr) {
        memcpy(new_arr, template_arr, count * sizeof(int64_t));
    }
    return new_arr;
}

int main() {
    printf("--- Testing Main Adaptive Sorter (select_and_sort) ---\n");
    int pass_count = 0;
    int total_tests = 0;
    
    // --- Define Template Arrays ---
    int64_t small_arr[] = {5, 1, 10, 2, 8}; // Should use Insertion
    int64_t small_exp[] = {1, 2, 5, 8, 10};
    size_t small_count = 5;

    int64_t sorted_arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // Should use Insertion (Low Entropy)
    int64_t sorted_exp[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t sorted_count = 10;
    
    int64_t dup_arr[] = {5, 5, 5, 5, 5, 5, 5, 5}; // Should use Insertion (Low Entropy)
    int64_t dup_exp[] = {5, 5, 5, 5, 5, 5, 5, 5};
    size_t dup_count = 8;
    
    int64_t neg_arr[] = {-5, -1, -10, 0, -2}; // Should use Radix
    int64_t neg_exp[] = {-10, -5, -2, -1, 0};
    size_t neg_count = 5;

    // Power of 2 sized array (to test Bitonic path logic)
    // On your machine, will fall back to Radix.
    int64_t bitonic_arr[] = {7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8};
    int64_t bitonic_exp[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    size_t bitonic_count = 16;
    
    // --- Run Tests ---

    total_tests++;
    int64_t* test_1 = create_array(small_arr, small_count);
    pass_count += check_sort("Small Array (Insertion path)", test_1, small_exp, small_count);
    free(test_1);

    total_tests++;
    int64_t* test_2 = create_array(sorted_arr, sorted_count);
    pass_count += check_sort("Already Sorted (Insertion path)", test_2, sorted_exp, sorted_count);
    free(test_2);

    total_tests++;
    int64_t* test_3 = create_array(dup_arr, dup_count);
    pass_count += check_sort("All Duplicates (Insertion path)", test_3, dup_exp, dup_count);
    free(test_3);
    
    total_tests++;
    int64_t* test_4 = create_array(neg_arr, neg_count);
    pass_count += check_sort("Negative Nums (Radix path)", test_4, neg_exp, neg_count);
    free(test_4);

    total_tests++;
    int64_t* test_5 = create_array(bitonic_arr, bitonic_count);
    pass_count += check_sort("Power-of-2 (Bitonic/Radix path)", test_5, bitonic_exp, bitonic_count);
    free(test_5);

    // --- Test 6: Large Random Array (Radix path) ---
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
        uint64_t high = ((uint64_t)rand() << 32);
        uint64_t low = (uint64_t)rand();
        uint64_t rand_64 = high | low;
        large_test_array[i] = (int64_t)rand_64; // Cast to signed
    }

    memcpy(large_expected_array, large_test_array, LARGE_SIZE * sizeof(int64_t));
    qsort(large_expected_array, LARGE_SIZE, sizeof(int64_t), qsort_compare);

    printf("Sorting 100000 elements with main ultra_sort...\n");
    pass_count += check_sort("Large Random Array (Radix path)", large_test_array, large_expected_array, LARGE_SIZE);
    free(large_test_array);
    free(large_expected_array);

    printf("--- Summary: %d / %d tests passed ---\n", pass_count, total_tests);

    return (pass_count == total_tests) ? 0 : 1;
}