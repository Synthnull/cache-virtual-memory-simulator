#include "page_table.h"
#include <stdio.h>
#include <stdlib.h>

Process *InitProcessPageTable(int initialSize, int maxCapacity,
										FILE *traceFile) {
	Process *processPtr = malloc(sizeof(Process));

	processPtr->tracefile = traceFile;
	processPtr->processPageTable = malloc(sizeof(PageTable));
	processPtr->processPageTable->maxCapacity = maxCapacity;
	processPtr->processPageTable->capacity = initialSize;
	processPtr->processPageTable->pages =
		 malloc(sizeof(PageTableEntry) * initialSize);

	return processPtr;
}

int addPage(int virAddr, int phyAddr, PageTable *pageTablePtr) {
	PageTableEntry *newPagePtr = malloc(sizeof(PageTableEntry));
	pageTablePtr->numPages++;
	if (pageTablePtr->numPages > pageTablePtr->maxCapacity) {
		free(newPagePtr);
		pageTablePtr->numPages--;
		return 1; // capacity miss
	}

	if (pageTablePtr->numPages > pageTablePtr->capacity) {
		pageTablePtr->pages =
			 realloc(pageTablePtr->pages, sizeof(PageTableEntry) * 8);
	}

	pageTablePtr->pages[pageTablePtr->numPages].phyAddr = phyAddr;
	pageTablePtr->pages[pageTablePtr->numPages].virAddr = phyAddr;
	pageTablePtr->pages[pageTablePtr->numPages].validBit = 1;

	return 0;
}
int removePageByVirAddr(int virAddr, int phyAddr, PageTable *pageTablePtr) {}
int removePageByPhyAddr(int virAddr, int phyAddr, PageTable *pageTablePtr) {}
int freeProcessPageTable(Process *processPtr) {}
