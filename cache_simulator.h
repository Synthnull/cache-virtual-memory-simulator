#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H
#include "cache_calculations.h"
#include "page_table.h"
#include "trace_parser.h"

typedef struct {
	int totalAccesses;
	int instructionBytes;
	int destBytes;
	int compulsoryMisses;
	int conflictMisses;
	int capacityMisses;
} CacheSimulationResults;

int runCacheSimulation(Process **processes, CacheOutput *cacheParameters,
							  int timeSlice, int numFiles,
							  CacheSimulationResults *results, TraceEntry entry);

#endif
