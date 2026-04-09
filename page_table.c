#include "page_table.h"
#include <stdbool.h>
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

int shiftPageTable(PageTable *pageTablePtr, int index) {
	PageTableEntry temp = pageTablePtr->pages[index];
	int i;
	for (i = 0; i < pageTablePtr->numPages - 1; i++) {
		pageTablePtr->pages[i] = pageTablePtr->pages[i + 1];
	}

	pageTablePtr->pages[pageTablePtr->numPages - 1] = temp;
	return 0;
}

int addPage(int virAddr, int phyAddr, PageTable *pageTablePtr) {
	pageTablePtr->numPages++;
	if (pageTablePtr->numPages > pageTablePtr->maxCapacity) {
		pageTablePtr->numPages--;
		return 1; // capacity miss
	}

	if (pageTablePtr->numPages > pageTablePtr->capacity) {
		pageTablePtr->pages =
			 realloc(pageTablePtr->pages, sizeof(PageTableEntry) * 8);
	}

	pageTablePtr->pages[pageTablePtr->numPages].phyAddr = phyAddr;
	pageTablePtr->pages[pageTablePtr->numPages].virAddr = phyAddr;
	pageTablePtr->pages[pageTablePtr->numPages].validBit = true;
   pageTablePtr->pages[pageTablePtr->numPages].dirtyBit = true;

	return 0;
}
bool removePageByVirAddr(int virAddr, PageTable *pageTablePtr) {
	int ind = searchPageByVir(pageTablePtr, virAddr);

	if (ind < 0) {
		return false; // no page to free
	}

	pageTablePtr->pages[ind].virAddr = 0;
	pageTablePtr->pages[ind].phyAddr = 0;
	pageTablePtr->pages[ind].validBit = 0;
	pageTablePtr->numPages--;
	shiftPageTable(pageTablePtr, ind);
	return true;
}
bool removePageByPhyAddr(int phyAddr, PageTable *pageTablePtr) {
	int ind = searchPageByPhy(pageTablePtr, phyAddr);

	if (ind < 0) {
		return false; // no page to free
	}

	pageTablePtr->pages[ind].virAddr = 0;
	pageTablePtr->pages[ind].phyAddr = 0;
	pageTablePtr->pages[ind].validBit = NULL;
   pageTablePtr->pages[ind].dirtyBit = NULL;
	pageTablePtr->numPages--;
	shiftPageTable(pageTablePtr, ind);
	return true;
}
int freeProcessPageTable(Process *processPtr) {
	free(processPtr->processPageTable->pages);
	free(processPtr);
	return 0;
}

int searchPageByVir(PageTable *pageTablePtr, int virAddr) {
	int i;
	for (i = 0; i < pageTablePtr->numPages; i++) {
		if (pageTablePtr->pages->virAddr == virAddr) {
			return i;
		}
	}

	return -1; // not found;
}

int searchPageByPhy(PageTable *pageTablePtr, int phyAddr) {
	int i;
	for (i = 0; i < pageTablePtr->numPages; i++) {
		if (pageTablePtr->pages->virAddr == phyAddr) {
			return i;
		}
	}

	return -1; // not found;
}
