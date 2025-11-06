#include "ultra_sort.h"
#include <stdio.h>
#include <string.h> // For memcmp
#include <stdlib.h> // For rand

// Helper function to print an array
void print_array(const char* title, int64_t* array, size_t count) {
    printf("%s [", title);
    for (size_t i = 0; i < count; i++) {
        printf("%lld", (long long)array[i]);
        if (i < count - 1) printf(", ");
    }
    printf("]\n");
}

// Helper function to check if two arrays are identical
// Returns 1 if pass, 0 if fail
int check_sort(const char* test_name, int64_t* array, int64_t* expected, size_t count) {
    // Call the assembly function via its C wrapper
    ultra_sort_insertion(array, count);

    if (memcmp(array, expected, count * sizeof(int64_t)) == 0) {
        printf("[PASS]: %s\n", test_name);
        return 1;
    } else {
        printf("[FAIL]: %s\n", test_name);
        print_array("  Got     ", array, count);
        print_array("  Expected", expected, count);
        return 0;
    }
}

int main() {
    printf("--- Testing Insertion Sort (insertion_sort_asm) ---\n");
    int pass_count = 0;
    int total_tests = 0;

    // Test 1: Reverse sorted
    int64_t reverse_array[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int64_t reverse_expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    total_tests++;
    pass_count += check_sort("Reverse Sorted", reverse_array, reverse_expected, 10);

    // Test 2: Already sorted
    int64_t sorted_array[] = {100, 200, 300, 400, 500};
    int64_t sorted_expected[] = {100, 200, 300, 400, 500};
    total_tests++;
    pass_count += check_sort("Already Sorted", sorted_array, sorted_expected, 5);

    // Test 3: All duplicates
    int64_t duplicate_array[] = {5, 5, 5, 5, 5};
    int64_t duplicate_expected[] = {5, 5, 5, 5, 5};
    total_tests++;
    pass_count += check_sort("All Duplicates", duplicate_array, duplicate_expected, 5);
    
    // Test 4: Small array with duplicates
    int64_t small_array[] = {5, 1, 10, 5, 2};
    int64_t small_expected[] = {1, 2, 5, 5, 10};
    total_tests++;
    pass_count += check_sort("Small Array w/ Duplicates", small_array, small_expected, 5);

    // Test 5: Empty array (edge case)
    int64_t empty_array[] = {};
    int64_t empty_expected[] = {};
    total_tests++;
    pass_count += check_sort("Empty Array", empty_array, empty_expected, 0);

    // Test 6: Single element (edge case)
    int64_t single_array[] = {42};
    int64_t single_expected[] = {42};
    total_tests++;
    pass_count += check_sort("Single Element", single_array, single_expected, 1);

    printf("--- Summary: %d / %d tests passed ---\n", pass_count, total_tests);

    return (pass_count == total_tests) ? 0 : 1; // Exit with error if any test failed
}
