#include "cache_simulator.h"
#include "address_parser.h"
#include "cpu_cache.h"
#include "cache_replace.h"
#include "error.h"

MissType runCacheSimulation(Cache *cachePtr, CacheOutput *cacheParameters,
                             CacheSimulationResults *results, int phyAddr,
                             char instType) {
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

    results->totalAccesses++;
    if (instType == 'R') {
        results->instructionBytes += cacheParameters->block_size;
    } else {
        results->destBytes += cacheParameters->block_size;
    }

    if (instType == 'R') {
        switch (missType) {
        case CONFLICT:
            results->conflictMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case COMPULSORY:
            results->compulsoryMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case CAPACITY:
            results->capacityMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case NO_MISS:
            break;
        }
    } else {
        switch (missType) {
        case CONFLICT:
            results->conflictMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
            break;
        case COMPULSORY:
            results->compulsoryMisses++;
            roundRobinReplace(cachePtr, index, tag, offset);
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
