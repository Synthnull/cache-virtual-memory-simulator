#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H

#include "cache_calculations.h"
#include "cmd_parser.h"
#include "cpu_cache.h"
#include "error.h"

typedef struct {
	int totalAccesses;
	int instructionBytes;
	int destBytes;
	int compulsoryMisses;
	int conflictMisses;
	int capacityMisses;
} CacheSimulationResults;

MissType runCacheSimulation(Cache *cachePtr, CacheOutput *cacheParameters,
									  CacheSimulationResults *results, int phyAddr,
									  char instType, ReplacementPolicy policy);

#endif
