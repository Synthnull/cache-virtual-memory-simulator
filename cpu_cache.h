#ifndef CPU_CACHE_H
#define CPU_CACHE_H

#include "cache_calculations.h"
#include "cache_simulator.h"
typedef enum { CAPACITY, CONFLICT, COMPULSORY, NO_MISS } MissType;

typedef struct {
	int tag;
	int validbit;
	int dirtybit;
} CacheBlock;

typedef struct {
	int associativity;
	int rows;
	int tagSize;
	int indexSize;
	CacheBlock **cacheBlocks;
} Cache;

Cache *initCache(CacheInput cacheInputParameters, CacheOutput cacheCalcResults);
MissType readCache(Cache *cachePtr, int phyAddr, int *cacheCol);
MissType writeCache(Cache * cachePtr, int phyAddr);
int freeCache(Cache *cachePtr);

#endif
