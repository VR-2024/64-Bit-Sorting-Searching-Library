#include "ultra_sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // For memset

// Helper to print test results
int check_entropy(const char* name, double value, double expected_low, double expected_high) {
    if (value >= expected_low && value <= expected_high) {
        printf("[PASS] Entropy (%s): %.4f\n", name, value);
        return 1;
    } else {
        printf("[FAIL] Entropy (%s): %.4f (Expected between %.4f and %.4f)\n", name, value, expected_low, expected_high);
        return 0;
    }
}

int main() {
    printf("--- Testing Utility Functions (Phase 2) ---\n");
    int pass_count = 0;
    int total_tests = 0;

    // --- 1. Testing Cache Detector (CPUID) ---
    total_tests++;
    printf("\n--- 1. Testing Cache Detector (CPUID) ---\n");
    ultra_cache_info_t cache_info;
    if (ultra_get_cache_info(&cache_info)) {
        printf("L1 Cache Size: %llu bytes (%llu KB)\n", (unsigned long long)cache_info.l1_size, (unsigned long long)cache_info.l1_size / 1024);
        printf("L2 Cache Size: %llu bytes (%llu KB)\n", (unsigned long long)cache_info.l2_size, (unsigned long long)cache_info.l2_size / 1024);
        printf("L3 Cache Size: %llu bytes (%llu KB)\n", (unsigned long long)cache_info.l3_size, (unsigned long long)cache_info.l3_size / 1024);
        printf("Cache Line Size: %llu bytes\n", (unsigned long long)cache_info.line_size);
        printf("[PASS] Cache detection ran successfully.\n");
        pass_count++;
    } else {
        printf("[FAIL] Cache detection (CPUID) failed.\n");
    }

    // --- 2. Testing Entropy Analyzer (POPCNT) ---
    printf("\n--- 2. Testing Entropy Analyzer (POPCNT) ---\n");
#define ENTROPY_SAMPLES 4096
    // Use static to ensure it's in .bss and large enough, though heap (malloc) is also fine.
    static int64_t test_data[ENTROPY_SAMPLES];
    double entropy;

    // Test 1: All Zeros (min entropy)
    total_tests++;
    memset(test_data, 0, sizeof(test_data));
    entropy = ultra_calculate_entropy(test_data, ENTROPY_SAMPLES);
    pass_count += check_entropy("All Zeros", entropy, 0.0, 0.0);

    // Test 2: All Ones (max entropy)
    total_tests++;
    memset(test_data, 0xFF, sizeof(test_data)); // 0xFF... = -1 (all 64 bits set)
    entropy = ultra_calculate_entropy(test_data, ENTROPY_SAMPLES);
    pass_count += check_entropy("All Ones", entropy, 1.0, 1.0);

    // Test 3: Alternating Bits (0x55... = 010101... -> 32 bits set)
    // (Perfect 0.5 entropy)
    total_tests++;
    memset(test_data, 0x55, sizeof(test_data)); // 0x5555...5555
    entropy = ultra_calculate_entropy(test_data, ENTROPY_SAMPLES);
    pass_count += check_entropy("Alternating Bits (0x55)", entropy, 0.5, 0.5);

    // Test 4: Low Entropy (Duplicates, 1 bit set)
    // (Expected: 1/64 = 0.015625)
    total_tests++;
    for(int i=0; i<ENTROPY_SAMPLES; i++) test_data[i] = 1; // Only one bit set
    entropy = ultra_calculate_entropy(test_data, ENTROPY_SAMPLES);
    pass_count += check_entropy("Low Entropy (All 1s)", entropy, 0.0156, 0.0157);

    // Test 5: Random Data (max entropy)
    total_tests++;
    srand(time(NULL));
    for (int i = 0; i < ENTROPY_SAMPLES; i++) {
        // Generate 64-bit random number
        test_data[i] = ((int64_t)rand() << 32) | rand();
    }
    entropy = ultra_calculate_entropy(test_data, ENTROPY_SAMPLES);
    // Should be very close to 0.5
    pass_count += check_entropy("Random Data", entropy, 0.48, 0.52); 
    
    printf("\n--- Summary: %d / %d utility tests passed ---\n", pass_count, total_tests);

    return (pass_count == total_tests) ? 0 : 1;
}
