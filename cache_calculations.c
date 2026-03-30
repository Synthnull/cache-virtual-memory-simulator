#include "cache_calculations.h"
#include <string.h>

#define KB 1024ULL
#define MB (1024ULL * 1024ULL)
#define PAGE_SIZE 4096ULL
#define VIRTUAL_ADDRESS_BITS 31
#define COST_PER_KB 0.07

static unsigned int log2_int(unsigned long long n) {
    unsigned int count = 0;
    while (n > 1) {
        n /= 2;
        count++;
    }
    return count;
}

int calculate(const CacheInput *input, CacheOutput *output) {
    unsigned long long cache_size_bytes;
    unsigned long long physical_memory_bytes;
    unsigned long long page_table_entries;
    unsigned long long total_page_table_bits;
    unsigned int block_offset_bits;
    unsigned int physical_address_bits;
    unsigned int physical_page_bits;

    if (input == 0 || output == 0) {
        return 0;
    }

    memset(output, 0, sizeof(CacheOutput));

    cache_size_bytes = (unsigned long long)input->cache_size_kb * KB;
    physical_memory_bytes = (unsigned long long)input->physical_memory_mb * MB;

    if (input->block_size == 0 || input->associativity == 0 || input->trace_file_count == 0) {
        return 0;
    }

    output->total_blocks = cache_size_bytes / input->block_size;
    output->total_rows = output->total_blocks / input->associativity;

    block_offset_bits = log2_int(input->block_size);
    output->index_size = log2_int(output->total_rows);
    physical_address_bits = log2_int(physical_memory_bytes);

    output->tag_size = physical_address_bits - output->index_size - block_offset_bits;

    output->overhead_size_bytes =
        (output->total_blocks * (output->tag_size + 1) + 7) / 8;

    output->implementation_memory_size_bytes =
        cache_size_bytes + output->overhead_size_bytes;

    output->implementation_memory_size_kb =
        (double)output->implementation_memory_size_bytes / 1024.0;

    output->cost = output->implementation_memory_size_kb * COST_PER_KB;

    output->number_of_physical_pages = physical_memory_bytes / PAGE_SIZE;

    output->number_of_pages_for_system =
        (unsigned long long)((output->number_of_physical_pages *
        input->percent_memory_used_by_system) / 100.0);

    physical_page_bits = log2_int(output->number_of_physical_pages);
    output->page_table_entry_size = 1 + physical_page_bits;

    page_table_entries = 1ULL << (VIRTUAL_ADDRESS_BITS - 12);

    total_page_table_bits =
        page_table_entries *
        input->trace_file_count *
        output->page_table_entry_size;

    output->total_ram_for_page_tables_bytes =
        (total_page_table_bits + 7) / 8;

    return 1;
}