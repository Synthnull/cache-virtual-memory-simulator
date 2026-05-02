#ifndef MISS_COUNTER_H
#define MISS_COUNTER_H



void processAccess(Cache *cachePtr, int phyAddr, char operation,
                   CacheSimulationResults *results);

#endif