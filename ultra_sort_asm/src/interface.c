#include "ultra_sort.h"
#include <stddef.h> // For NULL
#include <stdint.h> // For uint32_t

// --- Assembly Function Declarations ---
// These tell the C compiler that these functions exist and are
// defined in our .asm files (which will be linked later).

// Main Sorter (The "Brain")
extern void select_and_sort(int64_t* array, uint64_t count);

// Individual Algorithms
extern void radix_sort_asm(int64_t* array, uint64_t count);
extern void bitonic_sort_avx512(int64_t* array, uint64_t count);
extern void insertion_sort_asm(int64_t* array, uint64_t count);

// Utility Functions
extern double calculate_entropy(int64_t* array, uint64_t count);
extern int detect_cache_sizes(ultra_cache_info_t* info); // Returns 1 on success, 0 on fail


// --- C Wrapper Function Implementations ---

// Main interface function (for Phase 6)
// This is the primary function users should call.
void ultra_sort(int64_t* array, size_t count) {
    if (array == NULL || count == 0) return;
    // Call the assembly "brain"
    select_and_sort(array, (uint64_t)count);
}

// Direct access to Radix Sort (for testing)
void ultra_sort_radix(int64_t* array, size_t count) {
    if (array == NULL || count == 0) return;
    radix_sort_asm(array, (uint64_t)count);
}

// Direct access to Bitonic Sort (for testing)
void ultra_sort_bitonic_avx512(int64_t* array, size_t count) {
    if (array == NULL || count == 0) return;
    bitonic_sort_avx512(array, (uint64_t)count);
}

// Direct access to Insertion Sort (for testing)
void ultra_sort_insertion(int64_t* array, size_t count) {
    if (array == NULL || count == 0) return;
    insertion_sort_asm(array, (uint64_t)count);
}

// C wrapper for the entropy calculator
double ultra_calculate_entropy(int64_t* array, size_t count) {
    if (array == NULL || count == 0) return 0.0;
    return calculate_entropy(array, (uint64_t)count);
}

// C wrapper for the cache detector
int ultra_get_cache_info(ultra_cache_info_t* info) {
    if (info == NULL) return 0;
    // Call the assembly cache detector
    return detect_cache_sizes(info);
}

// C-based function to check for AVX-512 support
// This uses GCC-specific inline assembly (as seen in the docs)
int ultra_detect_avx512_support(void) {
    uint32_t eax, ebx, ecx, edx;
    
    // Call CPUID leaf 7, subleaf 0
    __asm__ __volatile__(
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (7), "c" (0)
    );
    
    // AVX-512F (Foundation) is bit 16 of the EBX register
    return (ebx & (1 << 16)) != 0;
}