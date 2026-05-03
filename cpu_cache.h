#ifndef CPU_CACHE_H
#define CPU_CACHE_H

#include "cache_calculations.h"
#include "error.h"
#include "page_table.h"

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
	int *nextReplace;
	CacheBlock **cacheBlocks;
} Cache;

Cache *initCache(int associativity, CacheOutput cacheCalcResults);
MissType readCache(Cache *cachePtr, int phyAddr, int *cacheCol);
int flushCache(Cache *cachePtr, PageTable *processPtr);
int freeCache(Cache *cachePtr);

#endif
