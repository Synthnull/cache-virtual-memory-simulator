#include "cpu_cache.h"
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
