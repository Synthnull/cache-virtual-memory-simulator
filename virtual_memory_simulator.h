#ifndef VIRTUAL_MEMORY_SIMULATION_H
#define VIRTUAL_MEMORY_SIMULATION_H
#include "memory_calculations.h"
#include "page_table.h"

typedef struct {
	int totalPages;
	int pagesAvaibletoUser;
	int pageHits;
	int pagesFree;
	int pageFaults;
} MemorySimulationResults;

int runVirtualMemorySimulation(Process **processes,
										 MemoryCalculationResults *pgTableParameters,
										 MemorySimulationResults *);

#endif
