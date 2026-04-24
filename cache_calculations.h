#ifndef CACHE_CALCULATIONS_H
#define CACHE_CALCULATIONS_H

#include "cmd_parser.h"
#include "trace_parser.h"

typedef struct {
	unsigned int cache_size_kb;
	unsigned int block_size;
	unsigned int associativity;
	unsigned int physical_memory_mb;
	double percent_memory_used_by_system;
	unsigned int trace_file_count;
} CacheInput;

typedef struct {
	unsigned long long total_blocks;
	unsigned long long total_rows;
	unsigned int tag_size;
	unsigned int index_size;
	unsigned long long overhead_size_bytes;
	unsigned long long implementation_memory_size_bytes;
	double implementation_memory_size_kb;
	double cost;
} CacheOutput;

int calculate_cache(const Parameters *input, CacheOutput *output, TraceEntry entry);

#endif
