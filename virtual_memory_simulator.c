#include "virtual_memory_simulator.h"
#include "trace_parser.h"
#include <stdlib.h>

#define PAGE_OFFSET_BITS 12
#define PAGE_OFFSET_MASK 0xFFF
#define INITIAL_EVICT_PROCESS 0

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

int runVirtualMemorySimulation(Process **processes,
										 MemoryCalculationResults *pgTableParameters,
										 int timeSlice,
										 MemorySimulationResults *results) {
	int numProcesses;
	int *finishedArray;
	int finishedCount;
	int nextEvictProcess;
	unsigned long long totalPhysicalPages;
	unsigned long long systemPages;
	unsigned long long pagesAvailableToUser;
	unsigned long long freePagesRemaining;
	unsigned long long nextFreePhysicalPage;
	if (processes == NULL || pgTableParameters == NULL || results == NULL) {
		return 0;
	}

	numProcesses = pgTableParameters->num_trace_files;

	if (numProcesses <= 0) {
		return 0;
	}

	finishedArray = calloc(numProcesses, sizeof(int));
	if (finishedArray == NULL) {
		return 0;
	}

	totalPhysicalPages = pgTableParameters->number_physical_pages;
	systemPages = pgTableParameters->number_pages_for_system;
	pagesAvailableToUser = totalPhysicalPages - systemPages;
	freePagesRemaining = pagesAvailableToUser;
	nextFreePhysicalPage = systemPages;
	nextEvictProcess = INITIAL_EVICT_PROCESS;
	finishedCount = 0;

	results->physicalPagesUsedBySystem = systemPages;
	results->pagesAvaibletoUser = pagesAvailableToUser;
	results->virtualPagesMapped = 0;
	results->pageHits = 0;
	results->pagesFromFree = 0;
	results->pageFaults = 0;

	while (finishedCount < numProcesses) {
		int processIndex;

		for (processIndex = 0; processIndex < numProcesses; processIndex++) {
			Process *currentProcess;
			PageTable *currentTable;
			int instructionsExecutedThisSlice;

			if (finishedArray[processIndex]) {
				continue;
			}

			currentProcess = processes[processIndex];
			if (currentProcess == NULL ||
				 currentProcess->processPageTable == NULL) {
				finishedArray[processIndex] = 1;
				finishedCount++;
				continue;
			}

			currentTable = currentProcess->processPageTable;
			instructionsExecutedThisSlice = 0;

			while (timeSlice == -1 || instructionsExecutedThisSlice < timeSlice) {
				TraceEntry entry;
				int virtualPageNumber;
				int offset;
				int entryIndex;

				if (!getNextTraceEntry(currentProcess->tracefile, &entry)) {
					freePagesRemaining += (unsigned long long)currentTable->numPages;
					finishedArray[processIndex] = 1;
					finishedCount++;
					break;
				}

				virtualPageNumber = (int)(entry.virAddr >> PAGE_OFFSET_BITS);
				offset = (int)(entry.virAddr & PAGE_OFFSET_MASK);

				entryIndex = searchPageByVir(currentTable, virtualPageNumber);

				if (entryIndex >= 0) {
					int physicalPageNumber;
					unsigned int physicalAddress;

					results->pageHits++;

					physicalPageNumber = currentTable->pages[entryIndex].phyAddr;
					physicalAddress =
						 ((unsigned int)physicalPageNumber << PAGE_OFFSET_BITS) |
						 (unsigned int)offset;
					(void)physicalAddress;
				} else {
					int physicalPageNumber;

					results->virtualPagesMapped++;

					if (freePagesRemaining > 0) {
						physicalPageNumber = (int)nextFreePhysicalPage;
						nextFreePhysicalPage++;
						freePagesRemaining--;

						addPage(virtualPageNumber, physicalPageNumber, currentTable);
						results->pagesFromFree++;
					} else {
						int victimProcessIndex;
						Process *victimProcess;
						PageTable *victimTable;
						results->pageFaults++;
						victimProcessIndex = findVictimProcess(
							 processes, finishedArray, numProcesses, nextEvictProcess);

						if (victimProcessIndex < 0) {
							free(finishedArray);
							return 0;
						}
                  
						victimProcess = processes[victimProcessIndex];
						victimTable = victimProcess->processPageTable;

						physicalPageNumber = victimTable->pages[0].phyAddr;

						removePageByPhyAddr(physicalPageNumber, victimTable);
						addPage(virtualPageNumber, physicalPageNumber, currentTable);

						nextEvictProcess = (victimProcessIndex + 1) % numProcesses;
					}
				}

				if (entry.instructionComplete) {
					instructionsExecutedThisSlice++;
				}
			}
		}
	}

	free(finishedArray);
	return 1;
}
