#ifndef MEMORY_CALCULATIONS_H
#define MEMORY_CALCULATIONS_H

typedef struct {
    int physical_memory_mb;
    double os_percent;
    int num_trace_files;

    unsigned long long number_physical_pages;
    unsigned long long number_pages_for_system;
    unsigned int page_table_entry_size_bits;
    unsigned long long total_ram_for_page_tables_bytes;
} MemoryCalculationResults;

int calculate(int physical_memory_mb,
              double os_percent,
              int num_trace_files,
              MemoryCalculationResults *results);

#endif
