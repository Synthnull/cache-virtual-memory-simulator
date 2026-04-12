#include "print.h"
#include "page_table.h"
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
	const char *policy =
		 (params->replacementPolicy == RR) ? "Random" : "Round Robin";
	printf("***** Cache Input Parameters *****\n\n");
	printf("%-30s %d KB\n", "Cache Size:", params->cacheSize);
	printf("%-30s %d bytes\n", "Block Size:", params->blockSize);
	printf("%-30s %d\n", "Associativity:", params->associativity);
	printf("%-30s %s\n", "Replacement Policy:", policy);
	printf("%-30s %d MB\n", "Physical Memory:", params->physicalMemory);
	printf("%-30s %.1f%%\n",
			 "Percent Memory Used by System:", (double)params->physicalMemoryOS);
	printf("%-30s %d\n\n", "Instructions / Time Slice:", params->timeSlice);
}

void printCacheCalculated(CacheOutput calc) {
	printf("***** Cache Calculated Values *****\n\n");
	printf("%-30s %llu\n", "Total # Blocks:", calc.total_blocks);
	printf("%-30s %u bits\n", "Tag Size:", calc.tag_size);
	printf("%-30s %u bits\n", "Index Size:", calc.index_size);
	printf("%-30s %llu\n", "Total # Rows:", calc.total_rows);
	printf("%-30s %llu bytes\n", "Overhead Size:", calc.overhead_size_bytes);
	printf("%-30s %.2f KB  (%llu bytes)\n",
			 "Implementation Memory Size:", calc.implementation_memory_size_kb,
			 calc.implementation_memory_size_bytes);
	printf("%-30s $%.2f @ $0.07 per KB\n\n", "Cost:", calc.cost);
}

void printPhysicalMemory(MemoryCalculationResults mem) {
	printf("***** Physical Memory Calculated Values *****\n\n");
	printf("%-30s %llu\n",
			 "Number of Physical Pages:", mem.number_physical_pages);
	printf("%-30s %llu\n",
			 "Number of Pages for System:", mem.number_pages_for_system);
	printf("%-30s %u bits\n",
			 "Size of Page Table Entry:", mem.page_table_entry_size_bits);
	printf("%-30s %llu bytes\n",
			 "Total RAM for Page Table(s):", mem.total_ram_for_page_tables_bytes);
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

void printVirMemorySimulationResults(MemorySimulationResults simResults, Process **processes, int numProcesses) {
   int i;
	printf("\n\n***** VIRTUAL MEMORY SIMULATION RESULTS *****\n\n");
	printf("%-30s %llu\n", "Physical Pages Used By SYSTEM:",
			 simResults.physicalPagesUsedBySystem);
	printf("%-30s %llu\n",
			 "Pages Avaiable To User:", simResults.pagesAvaibletoUser);
	printf("%-30s %llu\n",
			 "Virtual Pages Mapped:", simResults.virtualPagesMapped);
   printf("    ---------------------------------");
	printf("\n    %-26s %llu\n", "Page Table Hits:", simResults.pageHits);
	printf("    %-26s %llu\n", "Pages From Free:", simResults.pagesFromFree);
   printf("    %-26s %llu\n", "Total Page Faults:", simResults.pageFaults);

   printf("\n\nPage Table Usage Per Process:");
   printf("\n-----------------------------\n");

   for(i = 0; i < numProcesses; i++) {
      printf("\n[%d] %s:", i, processes[i]->fileName);
      printf("\n    Used Page Table Entries: %d (%f %%)", processes[i]->processPageTable->numPages, (processes[i]->processPageTable->numPages / (float)simResults.pagesAvaibletoUser) / 100);
      printf("\n    Page Table Wasted: %llu bytes", 4096 * (simResults.pagesAvaibletoUser - processes[i]->processPageTable->numPages));
      printf("\n");
   }
   printf("\n");
}
