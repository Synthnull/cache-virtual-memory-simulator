#include "cache_simulator.h"
#include "address_parser.h"
#include "cache_replace.h"
#include "cmd_parser.h"
#include "cpu_cache.h"
#include "error.h"

static void replaceCacheBlock(Cache *cachePtr, ReplacementPolicy policy,
										unsigned int index, unsigned int tag,
										unsigned int offset) {
	if (policy == RND) {
		randomReplace(cachePtr, index, tag, offset);
	} else {
		roundRobinReplace(cachePtr, index, tag, offset);
	}
}

MissType runCacheSimulation(Cache *cachePtr, CacheOutput *cacheParameters,
									 CacheSimulationResults *results,
									 unsigned int phyAddr, char instType,
									 int instSize, int isInstruction,
									 ReplacementPolicy policy, int blockSize) {
	MissType missType = NO_MISS;
	int cacheCol = 0;
	unsigned int tag;
	unsigned int index;
	unsigned int offset;
	unsigned int firstBlock;
	unsigned int lastBlock;
	unsigned int currentBlock;
	unsigned int accessAddr;
	int blockOffset;
	int accessSize;

	(void)instType;

	if (cachePtr == NULL || cacheParameters == NULL || results == NULL) {
		return NO_MISS;
	}

	accessSize = isInstruction ? instSize : 4;

	if (accessSize <= 0) {
		accessSize = 4;
	}

	results->totalAddresses++;

	if (isInstruction) {
		results->totalInstructions++;
		results->instructionBytes += accessSize;
		results->totalCycles += 2;
	} else {
		results->destBytes += accessSize;
		results->totalCycles += 1;
	}

	blockOffset = 32 - (cachePtr->indexSize + cachePtr->tagSize);

	firstBlock = phyAddr >> blockOffset;
	lastBlock = (phyAddr + (unsigned int)accessSize - 1) >> blockOffset;

	for (currentBlock = firstBlock; currentBlock <= lastBlock; currentBlock++) {
		results->totalAccesses++;

		accessAddr = currentBlock << blockOffset;

		parseAddress(accessAddr, &tag, &index, &offset,
						 cacheParameters->tag_size,
						 cacheParameters->index_size);

		missType = readCache(cachePtr, accessAddr, &cacheCol);

		if (missType == NO_MISS) {
			results->totalCycles += 1;
		} else {
			int memoryReads = (blockSize + 3) / 4;
			results->totalCycles += 4 * memoryReads;
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
	}

	return missType;
}
