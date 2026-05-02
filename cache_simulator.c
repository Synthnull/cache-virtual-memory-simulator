#include "cache_simulator.h"
#include "address_parser.h"
#include "cpu_cache.h"
#include "cache_replace.h"
#include "error.h"

MissType runCacheSimulation(Cache *cachePtr, CacheOutput *cacheParameters,
                             CacheSimulationResults *results, int phyAddr, char instType) {
    MissType missType;
    int cacheCol = 0;
    int tag;
    int index;
    int offset;

    if (cachePtr == NULL || cacheParameters == NULL || results == NULL)
        return NO_MISS;

    parseAddress(phyAddr, &tag, &index, &offset, cacheParameters->tag_size,
                 cacheParameters->index_size);

    missType = readCache(cachePtr, phyAddr, &cacheCol);

    if (instType == 'R') {
        results->totalAccesses++;
        results->instructionBytes += cacheParameters->block_size;
        switch (missType) {
        case CONFLICT:
            results->conflictMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case COMPULSORY:
            results->compulsoryMisses++;
            cachePtr->cacheBlocks[index][cacheCol].tag     = tag;
            cachePtr->cacheBlocks[index][cacheCol].validbit = 1;
            break;
        case CAPACITY:
            results->capacityMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case NO_MISS:
            break;
        }
    } else {
        results->totalAccesses++;
        results->destBytes += cacheParameters->block_size;
        switch (missType) {
        case CONFLICT:
            results->conflictMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case COMPULSORY:
            results->compulsoryMisses++;
            cachePtr->cacheBlocks[index][cacheCol].tag      = tag;
            cachePtr->cacheBlocks[index][cacheCol].validbit = 1;
            break;
        case CAPACITY:
            results->capacityMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case NO_MISS:
            break;
        }
    }

    return missType;
}
