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
									 unsigned int phyAddr, char instType, int instSize,
									 ReplacementPolicy policy, int blockSize) {
	MissType missType;
	int cacheCol = 0;
	unsigned int tag;
	unsigned int index;
	unsigned int offset;

	if (cachePtr == NULL || cacheParameters == NULL || results == NULL) {
		return NO_MISS;
	}

	// check multi block instruction:
   int blockOffset = 32 - (cachePtr->indexSize + cachePtr->tagSize);
	unsigned int firstBlock = phyAddr >> blockOffset;
	unsigned int lastBlock = (phyAddr + instSize) >> blockOffset;
	results->totalAccesses++;
	for (unsigned int currentBlock = firstBlock; currentBlock <= lastBlock;
		  currentBlock++) {
		unsigned int accessAddr = currentBlock << blockOffset;

		parseAddress(accessAddr, &tag, &index, &offset, cacheParameters->tag_size,
						 cacheParameters->index_size);

		missType = readCache(cachePtr, accessAddr, &cacheCol);

		if (missType == NO_MISS) {
			results->totalCycles += 1;
		} else {
			int memoryReads = (blockSize + 3) / 4;
			results->totalCycles += 4 * memoryReads;
		}

		if (instType == 'R') {
			results->instructionBytes += instSize;
			results->totalInstructions++;
			results->totalCycles += 2;
		} else {
			results->destBytes += instSize;
			results->totalCycles += 1;
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
			break;

		case CAPACITY:
			results->capacityMisses++;
			replaceCacheBlock(cachePtr, policy, index, tag, offset);
			break;

		case NO_MISS:
			break;
		}
	}

	return missType;
}
