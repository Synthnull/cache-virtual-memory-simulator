#include "virtual_memory_simulator.h"
#include "cpu_cache.h"
#include "page_table.h"
#include "trace_parser.h"
#include "address_parser.h"
#include <stdlib.h>

static int findVictimProcess(Process **processes, int *finishedArray,
									  int numProcesses, int startIndex) {
	int checked = 0;
	int idx = startIndex;

	while (checked < numProcesses) {
		if (!finishedArray[idx] && processes[idx] != NULL &&
			 processes[idx]->processPageTable != NULL &&
			 processes[idx]->processPageTable->numPages > 0) {
			return idx;
		}

		idx = (idx + 1) % numProcesses;
		checked++;
	}

	return -1;
}

MemoryReturnStatus runVirtualMemorySimulation(
	 Process **processes, int processIndex,
	 MemoryCalculationResults *pgTableParameters, int timeSlice,
	 MemorySimulationResults *results, MemoryState *state, TraceEntry entry,
	 int numProcesses, PagesAffected *pagesAffected, Cache *cachePtr) {
	Process *currentProcess;
	PageTable *currentTable;

	if (state->finishedArray[processIndex]) {
		return PROC_SKIP;
	}

	currentProcess = processes[processIndex];
	currentTable = currentProcess->processPageTable;
	if (currentProcess == NULL || currentProcess->processPageTable == NULL) {
		state->finishedArray[processIndex] = 1;
		state->finishedCount++;
		return PROC_FINISHED;
	}

	int virtualPageNumber;
	int offset;
	int entryIndex;

	virtualPageNumber = (int)(entry.virAddr >> PAGE_OFFSET_BITS);
	offset = (int)(entry.virAddr & PAGE_OFFSET_MASK);

	entryIndex = searchPageByVir(currentTable, virtualPageNumber);

	if (entryIndex >= 0) {
		int physicalPageNumber;
		unsigned int physicalAddress;

		results->pageHits++;
		pagesAffected->addedIdx = entryIndex;
		physicalPageNumber = currentTable->pages[entryIndex].phyAddr;
		physicalAddress = ((unsigned int)physicalPageNumber << PAGE_OFFSET_BITS) |
								(unsigned int)offset;
		(void)physicalAddress;
	} else {
		int physicalPageNumber;

		results->virtualPagesMapped++;

		if (state->freePagesRemaining > 0) {
			physicalPageNumber = (int)state->nextFreePhysicalPage;
			state->nextFreePhysicalPage++;
			state->freePagesRemaining--;

			addPage(virtualPageNumber, physicalPageNumber, currentTable);
			pagesAffected->addedIdx =
				 processes[processIndex]->processPageTable->numPages - 1;
			results->pagesFromFree++;
			processes[processIndex]->numPagesAtTermination++;
		} else {
			int victimProcessIndex;
			int removedPageIndex;
			Process *victimProcess;
			PageTable *victimTable;

			results->pageFaults++;
			victimProcessIndex =
				 findVictimProcess(processes, state->finishedArray, numProcesses,
										 state->nextEvictProcess);

			if (victimProcessIndex < 0) {
				free(state->finishedArray);
				return ERR;
			}

			victimProcess = processes[victimProcessIndex];
			victimTable = victimProcess->processPageTable;

			unsigned int physicalPageNumber = victimTable->pages[0].phyAddr;

			unsigned int pageOffset = entry.virAddr & PAGE_OFFSET_MASK;
			unsigned int fullPhyAddr = (physicalPageNumber << PAGE_OFFSET_BITS) | pageOffset;
         int j;
         unsigned int tag;
         unsigned int offset;
         unsigned int index;
         int currCacheCol;
			for (j = 0; j < 4096; j++) {
				unsigned int currPhyAddr = fullPhyAddr + j;
				parseAddress(currPhyAddr, &tag, &index, &offset, cachePtr->tagSize,
								 cachePtr->indexSize);

				if (readCache(cachePtr, currPhyAddr, &currCacheCol) == NO_MISS) {
					cachePtr->cacheBlocks[index][currCacheCol].validbit = 0;
				}
			}

			removedPageIndex =
				 removePageByPhyAddr(physicalPageNumber, victimTable);
			addPage(virtualPageNumber, physicalPageNumber, currentTable);
			processes[processIndex]->numPagesAtTermination++;

			pagesAffected->addedIdx =
				 processes[processIndex]->processPageTable->numPages - 1;
			pagesAffected->removedIdx = removedPageIndex;

			state->nextEvictProcess = (victimProcessIndex + 1) % numProcesses;
		}
	}

	return SUCCESS;
}
