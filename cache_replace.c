#include "cache_replace.h"
#include <stdlib.h>

/*** return index/column of cache block replaced *****/
int roundRobinReplace(Cache *cachePtr, unsigned int index, unsigned int tag,
							 unsigned int offset) {
	int victimCol;

	(void)offset;

	if (cachePtr == NULL || index < 0 || index >= cachePtr->rows) {
		return -1;
	}

	victimCol = cachePtr->nextReplace[index];

	cachePtr->cacheBlocks[index][victimCol].tag = tag;
	cachePtr->cacheBlocks[index][victimCol].validbit = 1;
	cachePtr->cacheBlocks[index][victimCol].dirtybit = 0;

	cachePtr->nextReplace[index] =
		 (cachePtr->nextReplace[index] + 1) % cachePtr->associativity;
	return victimCol;
}

/*** return index/column of cache block replaced ****/
int randomReplace(Cache *cachePtr, unsigned int index, unsigned int tag,
						unsigned int offset) {
	int victimCol;

	(void)offset;

	if (cachePtr == NULL || index < 0 || index >= cachePtr->rows) {
		return -1;
	}

	victimCol = rand() % cachePtr->associativity;

	cachePtr->cacheBlocks[index][victimCol].tag = tag;
	cachePtr->cacheBlocks[index][victimCol].validbit = 1;
	cachePtr->cacheBlocks[index][victimCol].dirtybit = 0;

	return victimCol;
}
