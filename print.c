#include "print.h"
#include <stdio.h>

void printHeader(int teamNumber) {
	printf("Cache Simulator - CS 3853 - Team %d\n\n", teamNumber);
}

void printTraceFiles(Parameters *params) {
	printf("Trace File(s):\n");
	for (int i = 0; i < params->files.numFiles; i++) {
		printf("\t%s\n", params->files.files[i].fileName);
	}

	printf("\n");
}

void printCacheInput(Parameters *params) {
	printf("***** Cache Input Parameters *****\n\n");
	printf("Cache Size: %d KB\n", params->cacheSize);
	printf("Block Size: %d bytes\n", params->blockSize);
	printf("Associativity: %d\n", params->associativity);
	printf("Replacement Policy: %s\n", "RND");
	printf("Physical Memory: %d MB\n", params->physicalMemory);
	printf("Percent Memory Used by System: %d%%\n", params->physicalMemoryOS);
	printf("Instructions / Time Slice: %d\n\n", params->timeSlice);
}

void printCacheCalculated(CacheOutput calc) {
	printf("***** Cache Calculated Values *****\n\n");
	printf("Total # Blocks: %llu\n", calc.total_blocks);
	printf("Tag Size: %d bits\n", calc.tag_size);
	printf("Index Size: %d bits\n", calc.index_size);
	printf("Total # Rows: %llu\n", calc.total_rows);
	printf("Overhead Size: %llu bytes\n", calc.overhead_size_bytes);
	printf("Implementation Memory Size: %.2f KB (%llu bytes)\n",
			 calc.implementation_memory_size_kb,
			 calc.implementation_memory_size_bytes);
	printf("Cost: $%.2f @ $0.07 per KB\n\n", calc.cost);
}

void printPhysicalMemory(MemoryCalculationResults mem) {
	printf("***** Physical Memory Calculated Values *****\n\n");
	printf("Number of Physical Pages: %llu\n", mem.number_physical_pages);
	printf("Number of Pages for System: %llu\n", mem.number_pages_for_system);
	printf("Size of Page Table Entry: %u bits\n",
			 mem.page_table_entry_size_bits);
	printf("Total RAM for Page Table(s): %llu bytes\n",
			 mem.total_ram_for_page_tables_bytes);
}

void printCalculationResults(int teamNumber, Parameters *params,
									  CacheOutput cacheCalc,
									  MemoryCalculationResults memResults) {
	printHeader(teamNumber);
	printTraceFiles(params);
	printCacheInput(params);
	printCacheCalculated(cacheCalc);
	printPhysicalMemory(memResults);
}
