#include "print.h"
#include <stdio.h>

void printHeader(int teamNumber) {
	printf("Cache Simulator - CS 3853 - Team %d\n\n", teamNumber);
}

void printTraceFiles(Parameters *params) {
	printf("Trace File(s):\n");
	for (int i = 0; i < params->files.numFiles; i++) {
		printf("        %s\n", params->files.files[i].fileName);
	}
	printf("\n");
}

void printCacheInput(Parameters *params) {
	const char *policy = (params->replacementPolicy == RR) ? "Round Robin" : "Random";
	printf("***** Cache Input Parameters *****\n\n");
	printf("%-30s %d KB\n",    "Cache Size:",                    params->cacheSize);
	printf("%-30s %d bytes\n", "Block Size:",                    params->blockSize);
	printf("%-30s %d\n",       "Associativity:",                 params->associativity);
	printf("%-30s %s\n",       "Replacement Policy:",            policy);
	printf("%-30s %d MB\n",    "Physical Memory:",               params->physicalMemory);
	printf("%-30s %.1f%%\n",   "Percent Memory Used by System:", (double)params->physicalMemoryOS);
	printf("%-30s %d\n\n",     "Instructions / Time Slice:",     params->timeSlice);
}

void printCacheCalculated(CacheOutput calc) {
	printf("***** Cache Calculated Values *****\n\n");
	printf("%-30s %llu\n",       "Total # Blocks:",              calc.total_blocks);
	printf("%-30s %u bits\n",    "Tag Size:",                    calc.tag_size);
	printf("%-30s %u bits\n",    "Index Size:",                  calc.index_size);
	printf("%-30s %llu\n",       "Total # Rows:",                calc.total_rows);
	printf("%-30s %llu bytes\n", "Overhead Size:",               calc.overhead_size_bytes);
	printf("%-30s %.2f KB  (%llu bytes)\n", "Implementation Memory Size:",
			 calc.implementation_memory_size_kb, calc.implementation_memory_size_bytes);
	printf("%-30s $%.2f @ $0.07 per KB\n\n", "Cost:", calc.cost);
}

void printPhysicalMemory(MemoryCalculationResults mem) {
	printf("***** Physical Memory Calculated Values *****\n\n");
	printf("%-30s %llu\n",       "Number of Physical Pages:",    mem.number_physical_pages);
	printf("%-30s %llu\n",       "Number of Pages for System:",  mem.number_pages_for_system);
	printf("%-30s %u bits\n",    "Size of Page Table Entry:",    mem.page_table_entry_size_bits);
	printf("%-30s %llu bytes\n", "Total RAM for Page Table(s):", mem.total_ram_for_page_tables_bytes);
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
