#include "page_table.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

Process *InitProcessPageTable(int initialSize, int maxCapacity, FILE *traceFile,
										char *name) {
	Process *processPtr = malloc(sizeof(Process));

	/*check if allocation failed*/
	if (processPtr == NULL) {
		return NULL;
	}

	processPtr->tracefile = traceFile;
   processPtr->fileName = name;
	processPtr->processPageTable = malloc(sizeof(PageTable));

	/*check if allocation failed*/
	if (processPtr->processPageTable == NULL) {
		free(processPtr);
		return NULL;
	}

	processPtr->processPageTable->numPages = 0;
	processPtr->processPageTable->maxCapacity = maxCapacity;
	processPtr->processPageTable->capacity = initialSize;
	processPtr->processPageTable->pages =
		 malloc(sizeof(PageTableEntry) * initialSize);

	/*check if allocation failed*/
	if (processPtr->processPageTable->pages == NULL) {
		free(processPtr->processPageTable);
		free(processPtr);
		return NULL;
	}

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
	int newCapacity;
	if (pageTablePtr == NULL) {
		return 1;
	}

	if (pageTablePtr->numPages >= pageTablePtr->maxCapacity) {
		pageTablePtr->numPages--;
		return 1; /* capacity miss */
	}

	if (pageTablePtr->numPages >= pageTablePtr->capacity) {
		newCapacity = pageTablePtr->capacity * 2;

		if (newCapacity > pageTablePtr->maxCapacity) {
			newCapacity = pageTablePtr->maxCapacity;
		}

		pageTablePtr->pages =
			 realloc(pageTablePtr->pages, sizeof(PageTableEntry) * newCapacity);
		pageTablePtr->capacity = newCapacity;
	}

	pageTablePtr->pages[pageTablePtr->numPages].phyAddr = phyAddr;
	pageTablePtr->pages[pageTablePtr->numPages].virAddr = virAddr;
	pageTablePtr->pages[pageTablePtr->numPages].validBit = true;
	pageTablePtr->pages[pageTablePtr->numPages].dirtyBit = false;
	pageTablePtr->numPages++;

	return 0;
}
bool removePageByVirAddr(int virAddr, PageTable *pageTablePtr) {
	int ind = searchPageByVir(pageTablePtr, virAddr);

	if (ind < 0) {
		return false; /* no page to free */
	}

	pageTablePtr->pages[ind].virAddr = 0;
	pageTablePtr->pages[ind].phyAddr = 0;
	pageTablePtr->pages[ind].validBit = false;
	pageTablePtr->pages->dirtyBit = false;
	pageTablePtr->numPages--;
	shiftPageTable(pageTablePtr, ind);
	return true;
}
bool removePageByPhyAddr(int phyAddr, PageTable *pageTablePtr) {
	int ind = searchPageByPhy(pageTablePtr, phyAddr);

	if (ind < 0) {
		return false; /* no page to free */
	}

	pageTablePtr->pages[ind].virAddr = 0;
	pageTablePtr->pages[ind].phyAddr = 0;
	pageTablePtr->pages[ind].validBit = false;
	pageTablePtr->pages[ind].dirtyBit = false;
	pageTablePtr->numPages--;
	shiftPageTable(pageTablePtr, ind);
	return true;
}
int freeProcessPageTable(Process *processPtr) {
	if (processPtr == NULL) {
		return 0;
	}

	if (processPtr->processPageTable != NULL) {
		free(processPtr->processPageTable->pages);
		free(processPtr->processPageTable);
	}

	free(processPtr);
	return 0;
}

int searchPageByVir(PageTable *pageTablePtr, int virAddr) {
	int i;

	if (pageTablePtr == NULL) {
		return -2;
	}

	for (i = 0; i < pageTablePtr->numPages; i++) {
		if (pageTablePtr->pages[i].virAddr == virAddr) {
			return i;
		}
	}

	return -1; /* not found */
}

int searchPageByPhy(PageTable *pageTablePtr, int phyAddr) {
	int i;

	if (pageTablePtr == NULL) {
		return -2;
	}

	for (i = 0; i < pageTablePtr->numPages; i++) {
		if (pageTablePtr->pages[i].phyAddr == phyAddr) {
			return i;
		}
	}
	return -1; /* not found */
}
