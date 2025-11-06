#ifndef ULTRA_SORT_H
#define ULTRA_SORT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Cache information structure (from documentation)
typedef struct {
    uint64_t l1_size;
    uint64_t l2_size;
    uint64_t l3_size;
    uint64_t line_size;
} ultra_cache_info_t;

// Main adaptive sorting function
void ultra_sort(int64_t* array, size_t count);

// Algorithm-specific functions
void ultra_sort_radix(int64_t* array, size_t count);
void ultra_sort_bitonic_avx512(int64_t* array, size_t count);
void ultra_sort_insertion(int64_t* array, size_t count);

// Utility functions
double ultra_calculate_entropy(int64_t* array, size_t count);
int ultra_detect_avx512_support(void);
int ultra_get_cache_info(ultra_cache_info_t* info);

#ifdef __cplusplus
}
#endif

#endif // ULTRA_SORT_H
