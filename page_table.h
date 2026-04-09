#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
	int phyAddr;
	int virAddr;
	bool validBit;
   bool dirtyBit;
} PageTableEntry;

typedef struct {
	int numPages;
	int capacity;
	int maxCapacity;
	PageTableEntry *pages;
} PageTable;

typedef struct {
	FILE *tracefile;
	PageTable *processPageTable;
} Process;

Process *InitProcessPageTable(int initialSize, int maxCapacity,
										FILE *traceFile);
int insertPage(int virAddr, int phyAddr, int validBit, PageTable *pageTablePtr);
int addPage(int virAddr, int phyAddr, PageTable *pageTablePtr);
bool removePageByVirAddr(int virAddr, PageTable *pageTablePtr);
bool removePageByPhyAddr(int phyAddr, PageTable *pageTablePtr);
int freeProcessPageTable(Process *processPtr);
int searchPageByVir(PageTable *pageTablePtr, int virAddr);
int searchPageByPhy(PageTable *pageTablePtr, int phyAddr);

#endif
