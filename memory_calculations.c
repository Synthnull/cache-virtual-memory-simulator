#include "memory_calculations.h"
#include <stddef.h>

#define PAGE_SIZE_BYTES 4096ULL
#define USER_VIRTUAL_ADDRESS_LIMIT_BITS 31U
#define VALID_BIT_COUNT 1U

static int is_power_of_two_ull(unsigned long long x) {
    return x != 0 && (x & (x - 1)) == 0;
}

static unsigned int log2_ull(unsigned long long x) {
    unsigned int result = 0;
    while (x > 1) {
        x >>= 1;
        result++;
    }
    return result;
}

static unsigned long long ceil_div_ull(unsigned long long numerator,
                                       unsigned long long denominator) {
    return (numerator + denominator - 1) / denominator;
}

int calculate(int physical_memory_mb,
              double os_percent,
              int num_trace_files,
              MemoryCalculationResults *results) {
    unsigned long long physical_memory_bytes;
    unsigned long long number_physical_pages;
    unsigned int physical_page_bits;
    unsigned long long page_table_entries_per_process;
    unsigned long long total_ram_for_page_tables_bits;
    unsigned long long total_ram_for_page_tables_bytes;
    unsigned long long number_pages_for_system;

    if (results == NULL) return -1;
    if (physical_memory_mb == 0 || num_trace_files == 0) return -2;
    if (os_percent < 0.0 || os_percent > 100.0) return -3;
    if (!is_power_of_two_ull((unsigned long long)physical_memory_mb)) return -4;

    physical_memory_bytes =
        (unsigned long long)physical_memory_mb * 1024ULL * 1024ULL;

    number_physical_pages = physical_memory_bytes / PAGE_SIZE_BYTES;

    if (!is_power_of_two_ull(number_physical_pages)) return -5;

    physical_page_bits = log2_ull(number_physical_pages);

    page_table_entries_per_process =
        1ULL << (USER_VIRTUAL_ADDRESS_LIMIT_BITS - 12U);

    results->page_table_entry_size_bits =
        VALID_BIT_COUNT + physical_page_bits;

    total_ram_for_page_tables_bits =
        page_table_entries_per_process *
        (unsigned long long)num_trace_files *
        (unsigned long long)results->page_table_entry_size_bits;

    total_ram_for_page_tables_bytes =
        ceil_div_ull(total_ram_for_page_tables_bits, 8ULL);

    number_pages_for_system =
        (unsigned long long)((os_percent / 100.0) * (double)number_physical_pages);

    results->physical_memory_mb = physical_memory_mb;
    results->os_percent = os_percent;
    results->num_trace_files = num_trace_files;
    results->number_physical_pages = number_physical_pages;
    results->number_pages_for_system = number_pages_for_system;
    results->total_ram_for_page_tables_bytes = total_ram_for_page_tables_bytes;

    return 0;
}
