#ifndef VIRTUAL_MEMORY_SIMULATOR_H
#define VIRTUAL_MEMORY_SIMULATOR_H

#include "memory_calculations.h"
#include "page_table.h"
#include "trace_parser.h"

typedef struct {
	unsigned long long physicalPagesUsedBySystem;
	unsigned long long pagesAvaibletoUser;
	unsigned long long virtualPagesMapped;
	unsigned long long pageHits;
	unsigned long long pagesFromFree;
	unsigned long long pageFaults;
} MemorySimulationResults;

typedef struct {
	int *finishedArray;
	int finishedCount;
	int nextEvictProcess;
	unsigned long long totalPhysicalPages;
	unsigned long long systemPages;
	unsigned long long pagesAvailableToUser;
	unsigned long long freePagesRemaining;
	unsigned long long nextFreePhysicalPage;
} MemoryState;

typedef enum {
   PROC_SKIP,
   PROC_FINISHED,
   ERR,
   SUCCESS,
} MemoryReturnState;

MemoryReturnState runVirtualMemorySimulation(Process **processes, int processIndex,
										 MemoryCalculationResults *pgTableParameters,
										 int timeSlice, MemorySimulationResults *results,
										 MemoryState *state, TraceEntry entry,
										 int numProcesses);

#endif
