#include "cache_simulator.h"
#include "address_parser.h"
#include "cache_replace.h"
#include "cmd_parser.h"
#include "cpu_cache.h"
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
									 CacheSimulationResults *results,
									 unsigned int phyAddr, char instType,
									 ReplacementPolicy policy, int blockSize) {
	MissType missType;
	int cacheCol = 0;
	unsigned int tag;
	unsigned int index;
	unsigned int offset;

	if (cachePtr == NULL || cacheParameters == NULL || results == NULL) {
		return NO_MISS;
	}

	parseAddress(phyAddr, &tag, &index, &offset, cacheParameters->tag_size,
					 cacheParameters->index_size);

	missType = readCache(cachePtr, phyAddr, &cacheCol);

	if (missType == NO_MISS) {
		results->totalCycles += 1;
	} else {
		int memoryReads = (blockSize + 3) / 4;
		results->totalCycles += 4 * memoryReads;
	}

	results->totalAccesses++;

	if (instType == 'R') {
		results->instructionBytes += blockSize;
		results->totalInstructions++;
		results->totalCycles += 2;
	} else {
		results->destBytes += blockSize;
		results->totalCycles += 1;
	}

	switch (missType) {
	case CONFLICT:
		results->conflictMisses++;
		replaceCacheBlock(cachePtr, policy, index, tag, offset);
		break;

	case COMPULSORY:
		results->compulsoryMisses++;
		replaceCacheBlock(cachePtr, policy, index, tag, offset);
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
