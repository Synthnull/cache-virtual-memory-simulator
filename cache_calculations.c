#include "cache_calculations.h"
#include "cmd_parser.h"
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

int calculate_cache(const Parameters *input, CacheOutput *output) {
	unsigned long long cache_size_bytes;
	unsigned long long physical_memory_bytes;
	unsigned int block_offset_bits;
	unsigned int physical_address_bits;

	if (input == 0 || output == 0) {
		return 0;
	}

	memset(output, 0, sizeof(CacheOutput));

	cache_size_bytes = (unsigned long long)input->cacheSize * KB;
	physical_memory_bytes = (unsigned long long)input->physicalMemory * MB;

	if (input->blockSize == 0 || input->associativity == 0 ||
		 input->files.numFiles == 0) {
		return 0;
	}

	output->total_blocks = cache_size_bytes / input->blockSize;
	output->total_rows = output->total_blocks / input->associativity;

	block_offset_bits = log2_int(input->blockSize);
	output->index_size = log2_int(output->total_rows);
	physical_address_bits = log2_int(physical_memory_bytes);

	output->tag_size =
		 32 - (output->index_size + block_offset_bits);

	output->overhead_size_bytes =
		 (output->total_blocks * (output->tag_size + 1) + 7) / 8;

	output->implementation_memory_size_bytes =
		 cache_size_bytes + output->overhead_size_bytes;

	output->implementation_memory_size_kb =
		 (double)output->implementation_memory_size_bytes / 1024.0;

	output->cost = output->implementation_memory_size_kb * COST_PER_KB;

	return 1;
}
