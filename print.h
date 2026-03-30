#ifndef PRINT_H
#define PRINT_H
#include "cmd_parser.h"
#include "memory_calculations.h"

typedef struct {
    int totalBlocks;
    int tagBits;
    int indexBits;
    int totalRows;
    int overheadSize;
    double implementationSizeKB;
    int implementationSizeBytes;
    double cost;
} CacheCalculated;


void printCalculationResults(
    int teamNumber,
    Parameters *params,
    CacheCalculated cacheCalc,
    MemoryCalculationResults memResults
);

#endif
