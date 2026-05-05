#include "cpu_cache.h"
#include "address_parser.h"
#include "cache_calculations.h"
#include "error.h"
#include "page_table.h"
#include "virtual_memory_simulator.h"
#include <stdbool.h>
#include <stdlib.h>

Cache *initCache(int associativity, CacheOutput cacheCalcResults) {
	int i;
	Cache *newCache = malloc(sizeof(Cache));

	if (newCache == NULL) {
		return NULL;
	}

	newCache->rows = cacheCalcResults.total_rows;
	newCache->associativity = associativity;
	newCache->indexSize = cacheCalcResults.index_size;
	newCache->tagSize = cacheCalcResults.tag_size;

	newCache->nextReplace = calloc(newCache->rows, sizeof(int));

	if (newCache->nextReplace == NULL) {
		free(newCache);
		return NULL;
	}

	newCache->cacheBlocks =
		 (CacheBlock **)calloc(newCache->rows, sizeof(CacheBlock *));

	if (newCache->cacheBlocks == NULL) {
		free(newCache->nextReplace);
		free(newCache);
		return NULL;
	}

	for (i = 0; i < newCache->rows; i++) {
		newCache->cacheBlocks[i] =
			 (CacheBlock *)calloc(newCache->associativity, sizeof(CacheBlock));

		if (newCache->cacheBlocks[i] == NULL) {
			while (--i >= 0) {
				free(newCache->cacheBlocks[i]);
			}

			free(newCache->cacheBlocks);
			free(newCache->nextReplace);
			free(newCache);
			return NULL;
		}
	}

	return newCache;
}

MissType readCache(Cache *cachePtr, int phyAddr, int *cacheCol) {
	unsigned int tag;
	unsigned int index;
	unsigned int offset;
	int i;
	bool foundEmpty = false;

	parseAddress(phyAddr, &tag, &index, &offset, cachePtr->tagSize,
					 cachePtr->indexSize);

	for (i = 0; i < cachePtr->associativity; i++) {
		if (tag == cachePtr->cacheBlocks[index][i].tag &&
			 cachePtr->cacheBlocks[index][i].validbit == 1) {
			*cacheCol = i;
			return NO_MISS;
		} else if (cachePtr->cacheBlocks[index][i].validbit == 0) {
			if (!foundEmpty) {
				*cacheCol = i;
				foundEmpty = true;
			}
		}
	}

	return foundEmpty ? COMPULSORY : CONFLICT;
}

int flushCache(Cache *cachePtr, PageTable *processPtr) {
	int i;
	int j;
	int currCacheCol;
	unsigned int index;
   unsigned int tag;
   unsigned int offset;

	if (cachePtr == NULL || processPtr == NULL) {
		return 1;
	}

	for (i = 0; i < processPtr->numPages; i++) {
		unsigned int PPN = processPtr->pages[i].phyAddr;
		unsigned int basePhyAddr = (PPN << PAGE_OFFSET_BITS);

		for (j = 0; j < 4096; j++) {
         unsigned int currPhyAddr = basePhyAddr + j;
			parseAddress(currPhyAddr, &tag, &index, &offset, cachePtr->tagSize,
							 cachePtr->indexSize);

			if (readCache(cachePtr, currPhyAddr, &currCacheCol) == NO_MISS) {
				cachePtr->cacheBlocks[index][currCacheCol].validbit = 0;
			}
		}
	}

	return 0;
}

int freeCache(Cache *cachePtr) {
	int i;

	if (cachePtr == NULL) {
		return 1;
	}

	for (i = 0; i < cachePtr->rows; i++) {
		if (cachePtr->cacheBlocks[i] != NULL) {
			free(cachePtr->cacheBlocks[i]);
		}
	}

	free(cachePtr->cacheBlocks);
	free(cachePtr->nextReplace);
	free(cachePtr);

	return 0;
}
