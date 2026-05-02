#include "miss_counter.h"

void processMiss(MissType missType, char instType, int accessBytes,
                 CacheSimulationResults *results) {

    if (results == NULL)
        return;

    
    results->totalAccesses++;

    
    if (instType == 'R') {
        results->instructionBytes += accessBytes;
    } else if (instType == 'W') {
        results->destBytes += accessBytes;
    }

    
    switch (missType) {
        case COMPULSORY:
            results->compulsoryMisses++;
            break;
        case CONFLICT:
            results->conflictMisses++;
            break;
        case CAPACITY:
            results->capacityMisses++;
            break;
        case NO_MISS:
            break;
    }
}
