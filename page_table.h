#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
  int phyAddr;
  int virAddr;
  int validBit;
} PageTableEntry;

typedef struct {
   int numPages;
   int capacity;
   int maxCapacity;
   PageTableEntry* pages; 
} PageTable;

typedef struct {
   FILE *tracefile;
   PageTable *processPageTable;
} Process;

#endif
