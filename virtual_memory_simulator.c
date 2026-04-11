#include "virtual_memory_simulator.h"
#include "trace_parser.h"

#define PAGE_OFFSET_BITS 12
#define PAGE_OFFSET_MASK 0xFFF
#define INITIAL_EVICT_PROCESS 0

static int findVictimProcess(Process **processes, int numProcesses, int startIndex) {
	int checked = 0;
	int idx = startIndex;

	while (checked < numProcesses) {
		if (processes[idx] != NULL &&
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
										 MemorySimulationResults *results) {
	int processIndex;
	int numProcesses;
	unsigned long long totalPhysicalPages;
	unsigned long long systemPages;
	unsigned long long pagesAvailableToUser;
	unsigned long long freePagesRemaining;
	unsigned long long nextFreePhysicalPage;
	int nextEvictProcess;
	TraceEntry entry;

	if (processes == NULL || pgTableParameters == NULL || results == NULL) {
		return 0;
	}

	numProcesses = pgTableParameters->num_trace_files;
	totalPhysicalPages = pgTableParameters->number_physical_pages;
	systemPages = pgTableParameters->number_pages_for_system;
	pagesAvailableToUser = totalPhysicalPages - systemPages;
	freePagesRemaining = pagesAvailableToUser;
	nextFreePhysicalPage = systemPages;
	nextEvictProcess = INITIAL_EVICT_PROCESS;

	results->physicalPagesUsedBySystem = systemPages;
	results->pagesAvaibletoUser = pagesAvailableToUser;
	results->virtualPagesMapped = 0;
	results->pageHits = 0;
	results->pagesFromFree = 0;
	results->pageFaults = 0;

	for (processIndex = 0; processIndex < numProcesses; processIndex++) {
		Process *currentProcess;
		PageTable *currentTable;

		if (processes[processIndex] == NULL) {
			continue;
		}

		currentProcess = processes[processIndex];
		currentTable = currentProcess->processPageTable;

		while (getNextTraceEntry(currentProcess->tracefile, &entry)) {
			int virtualPageNumber;
			int offset;
			int entryIndex;

			virtualPageNumber = (int)(entry.virAddr >> PAGE_OFFSET_BITS);
			offset = (int)(entry.virAddr & PAGE_OFFSET_MASK);
			(void)offset;

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

					victimProcessIndex =
						 findVictimProcess(processes, numProcesses, nextEvictProcess);

					if (victimProcessIndex < 0) {
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
		}
	}

	return 1;
}
