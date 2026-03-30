#include <stdio.h>
#include "print.h"


void printHeader(int teamNumber) {
    printf("Cache Simulator - CS 3853 - Team 12%d\n", teamNumber);
}

void printTraceFiles(Parameters *params) {
    printf("Trace File(s):\n");
    for (int i = 0; i < params->files.numFiles; i++) {
        printf("%s\n", params->files.files[i].fileName);
    }
}

void printCacheInput(Parameters *params) {
    printf("***** Cache Input Parameters *****\n");
    printf("Cache Size: %d KB\n", params->cacheSize);
    printf("Block Size: %d bytes\n", params->blockSize);
    printf("Associativity: %d\n", params->associativity);
    printf("Replacement Policy: %s\n",
           "RND");
    printf("Physical Memory: %d MB\n", params->physicalMemory);
    printf("Percent Memory Used by System: %d%%\n",
           params->physicalMemoryOS);
    printf("Instructions / Time Slice: %d\n", params->timeSlice);
}

void printCacheCalculated(CacheCalculated calc) {
    printf("***** Cache Calculated Values *****\n");
    printf("Total # Blocks: %d\n", calc.totalBlocks);
    printf("Tag Size: %d bits\n", calc.tagBits);
    printf("Index Size: %d bits\n", calc.indexBits);
    printf("Total # Rows: %d\n", calc.totalRows);
    printf("Overhead Size: %d bytes\n", calc.overheadSize);
    printf("Implementation Memory Size: %.2f KB (%d bytes)\n",
           calc.implementationSizeKB, calc.implementationSizeBytes);
    printf("Cost: $%.2f @ $0.07 per KB\n", calc.cost);
}

void printPhysicalMemory(MemoryCalculationResults mem) {
    printf("***** Physical Memory Calculated Values *****\n");
    printf("Number of Physical Pages: %llu\n",
           mem.number_physical_pages);
    printf("Number of Pages for System: %llu\n",
           mem.number_pages_for_system);
    printf("Size of Page Table Entry: %u bits\n",
           mem.page_table_entry_size_bits);
    printf("Total RAM for Page Table(s): %llu bytes\n",
           mem.total_ram_for_page_tables_bytes);
}



void printCalculationResults(
    int teamNumber,
    Parameters *params,
    CacheCalculated cacheCalc,
    MemoryCalculationResults memResults
) {
    printHeader(teamNumber);
    printTraceFiles(params);
    printCacheInput(params);
    printCacheCalculated(cacheCalc);
    printPhysicalMemory(memResults);
}
