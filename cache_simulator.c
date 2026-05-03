#include "cache_simulator.h"
#include "address_parser.h"
#include "cpu_cache.h"
#include "cache_replace.h"
#include "cmd_parser.h"
#include "error.h"

static void replaceCacheBlock(Cache *cachePtr, ReplacementPolicy policy,
										int index, int tag, int offset) {
	if (policy == RND) {
		randomReplace(cachePtr, index, tag, offset);
	} else {
		roundRobinReplace(cachePtr, index, tag, offset);
	}
}

MissType runCacheSimulation(Cache *cachePtr, CacheOutput *cacheParameters,
									  CacheSimulationResults *results, int phyAddr,
									  char instType, ReplacementPolicy policy) {
	MissType missType;
	int cacheCol = 0;
	int tag;
	int index;
	int offset;

	if (cachePtr == NULL || cacheParameters == NULL || results == NULL) {
		return NO_MISS;
	}

	parseAddress(phyAddr, &tag, &index, &offset, cacheParameters->tag_size,
					 cacheParameters->index_size);

	missType = readCache(cachePtr, phyAddr, &cacheCol);

	results->totalAccesses++;

	if (instType == 'R') {
		results->instructionBytes += cacheParameters->block_size;
	} else {
		results->destBytes += cacheParameters->block_size;
	}

	switch (missType) {
	case CONFLICT:
		results->conflictMisses++;
		replaceCacheBlock(cachePtr, policy, index, tag, offset);
		break;

	case COMPULSORY:
		results->compulsoryMisses++;
		cachePtr->cacheBlocks[index][cacheCol].tag = tag;
		cachePtr->cacheBlocks[index][cacheCol].validbit = 1;
		cachePtr->cacheBlocks[index][cacheCol].dirtybit = 0;
		break;

	case CAPACITY:
		results->capacityMisses++;
		replaceCacheBlock(cachePtr, policy, index, tag, offset);
		break;

	case NO_MISS:
		break;
	}

	return missType;
}
