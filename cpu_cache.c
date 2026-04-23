#include "cpu_cache.h"
#include "address_parser.h"
#include <stdbool.h>
#include <stdlib.h>

Cache *initCache(CacheInput cacheInputParameters,
					  CacheOutput cacheCalcResults) {
	int i;
	Cache *newCache = malloc(sizeof(Cache));

	if (newCache == NULL) {
		return NULL; // alloc err
	}

	newCache->rows = cacheCalcResults.total_rows;
	newCache->associativity = cacheInputParameters.associativity;
	newCache->indexSize = cacheCalcResults.index_size;
	newCache->tagSize = cacheCalcResults.tag_size;

	newCache->cacheBlocks =
		 (CacheBlock **)calloc(newCache->rows, sizeof(CacheBlock *));

	for (i = 0; i < newCache->rows; i++) {
		newCache->cacheBlocks[i] =
			 (CacheBlock *)calloc(newCache->associativity, sizeof(CacheBlock));
		if (newCache->cacheBlocks[i] == NULL) {
			return NULL;
		}
	}
	return newCache;
}

MissType readCache(Cache *cachePtr, int phyAddr, int *cacheCol) {
	int tag;
	int index;
	int offset;
	int i;
	bool foundEmpty = false;

	parseAddress(phyAddr, &tag, &index, &offset, cachePtr->tagSize,
					 cachePtr->indexSize);

	for (i = 0; i < cachePtr->associativity; i++) {
		if (tag == cachePtr->cacheBlocks[index][i].tag &&
			 cachePtr->cacheBlocks[index][i].validbit == 1) {
			return NO_MISS; // hit
		} else if (cachePtr->cacheBlocks[index][i].validbit == 0) {
			foundEmpty = true; // no data
		}
	}
	return foundEmpty ? COMPULSORY : CONFLICT; // index full
}

MissType writeCache(Cache *cachePtr, int phyAddr) {
	int tag;
	int index;
	int offset;
	int i;
   bool foundEmpty = false;

	parseAddress(phyAddr, &tag, &index, &offset, cachePtr->tagSize,
					 cachePtr->indexSize);

	for (i = 0; i < cachePtr->associativity; i++) {
		if (tag == cachePtr->cacheBlocks[index][i].tag &&
			 cachePtr->cacheBlocks[index][i].validbit == 1) {
			return NO_MISS; // hit
         cachePtr->cacheBlocks[index][i].dirtybit = 1;
		} else if (cachePtr->cacheBlocks[index][i].validbit == 0) {
			foundEmpty = true; // no data
		}
	}

	return foundEmpty ? COMPULSORY : CONFLICT;
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
	free(cachePtr);
	return 0;
}
