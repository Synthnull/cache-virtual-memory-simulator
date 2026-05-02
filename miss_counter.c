#include "miss_counter.h"



static int getEvictCol(Cache *cachePtr, int index) {
    
    static int *rrCounters = NULL;
    static int  rrRows     = 0;

    
    if (rrCounters == NULL || rrRows != cachePtr->rows) {
        if (rrCounters != NULL) free(rrCounters);
        rrCounters = calloc(cachePtr->rows, sizeof(int));
        rrRows     = cachePtr->rows;
    }

    int col = rrCounters[index];
    rrCounters[index] = (rrCounters[index] + 1) % cachePtr->associativity;
    return col;
}


static void loadBlock(Cache *cachePtr, int tag, int index, char operation) {
    int i;

    
    for (i = 0; i < cachePtr->associativity; i++) {
        if (cachePtr->cacheBlocks[index][i].validbit == 0) {
            cachePtr->cacheBlocks[index][i].tag      = tag;
            cachePtr->cacheBlocks[index][i].validbit = 1;
            cachePtr->cacheBlocks[index][i].dirtybit = (operation == 'W') ? 1 : 0;
            return;
        }
    }

    
    int col = getEvictCol(cachePtr, index);
    cachePtr->cacheBlocks[index][col].tag      = tag;
    cachePtr->cacheBlocks[index][col].validbit = 1;
    cachePtr->cacheBlocks[index][col].dirtybit = (operation == 'W') ? 1 : 0;
}

void processAccess(Cache *cachePtr, int phyAddr, char operation,
                   CacheSimulationResults *results) {
    int tag, index, offset;

    parseAddress(phyAddr, &tag, &index, &offset,
                 cachePtr->tagSize, cachePtr->indexSize);

    
    int i;
    for (i = 0; i < cachePtr->associativity; i++) {
        if (cachePtr->cacheBlocks[index][i].validbit == 1 &&
            cachePtr->cacheBlocks[index][i].tag == tag) {

            
            if (operation == 'W')
                cachePtr->cacheBlocks[index][i].dirtybit = 1;
            return; 
        }
    }

    
    int foundEmpty = 0;
    for (i = 0; i < cachePtr->associativity; i++) {
        if (cachePtr->cacheBlocks[index][i].validbit == 0) {
            foundEmpty = 1;
            break;
        }
    }

    if (foundEmpty) {
        results->compulsoryMisses++;  
    } else {
        results->conflictMisses++;   
    }

    
    loadBlock(cachePtr, tag, index, operation);
}