#include <stdlib.h>

#include "cache_calculations.h"
#include "cache_simulator.h"
#include "cpu_cache.h"
#include "page_table.h"
#include "sim_runner.h"
#include "trace_parser.h"
#include "virtual_memory_simulator.h"

#define INITIAL_EVICT_PROCESS 0

int runSimulation(Parameters *parameters, MemoryCalculationResults *memResults,
						int associativity, CacheOutput *cacheResults,
						MemorySimulationResults *memSimResults,
						CacheSimulationResults *cacheSimResults,
						Process **processes) {
	int i;
	Cache *cache;
	MemoryState state = {0};
	int numProcesses;
	int instructionsExecutedThisSlice;
	int timeSlice;

	if (processes == NULL || parameters == NULL || memResults == NULL ||
		 cacheResults == NULL || memSimResults == NULL ||
		 cacheSimResults == NULL) {
		return 1;
	}

	for (i = 0; i < parameters->files.numFiles; i++) {
		processes[i] =
			 InitProcessPageTable(10,
										 memResults->number_physical_pages -
											  memResults->number_pages_for_system,
										 parameters->files.files[i].filePtr,
										 parameters->files.files[i].fileName);

		if (processes[i] == NULL) {
			while (--i >= 0) {
				freeProcessPageTable(processes[i]);
			}
			return 1;
		}
	}

	cache = initCache(associativity, *cacheResults);

	if (cache == NULL) {
		return 1;
	}

	numProcesses = parameters->files.numFiles;
	timeSlice = parameters->timeSlice;

	state.finishedArray = calloc(numProcesses, sizeof(int));

	if (state.finishedArray == NULL) {
		freeCache(cache);
		return 1;
	}

	state.totalPhysicalPages = memResults->number_physical_pages;
	state.systemPages = memResults->number_pages_for_system;
	state.pagesAvailableToUser = state.totalPhysicalPages - state.systemPages;
	state.freePagesRemaining = state.pagesAvailableToUser;
	state.nextFreePhysicalPage = state.systemPages;
	state.nextEvictProcess = INITIAL_EVICT_PROCESS;
	state.finishedCount = 0;

	memSimResults->physicalPagesUsedBySystem = state.systemPages;
	memSimResults->pagesAvaibletoUser = state.pagesAvailableToUser;
	memSimResults->virtualPagesMapped = 0;
	memSimResults->pageHits = 0;
	memSimResults->pagesFromFree = 0;
	memSimResults->pageFaults = 0;

	while (state.finishedCount < numProcesses) {
		int processIndex;

		for (processIndex = 0; processIndex < numProcesses; processIndex++) {
			Process *currentProcess;
			PageTable *currentTable;

			currentProcess = processes[processIndex];

			if (state.finishedArray[processIndex]) {
				continue;
			}

			if (currentProcess == NULL ||
				 currentProcess->processPageTable == NULL) {
				state.finishedArray[processIndex] = 1;
				state.finishedCount++;
				continue;
			}

			currentTable = currentProcess->processPageTable;
			instructionsExecutedThisSlice = 0;

			while (timeSlice == -1 || instructionsExecutedThisSlice < timeSlice) {
				TraceEntry entry;
				MemoryReturnStatus memStatus;
				PagesAffected affectedPages = {0};
				unsigned int PPN;
				unsigned int pageOffset;
				unsigned int phyAddr;

				if (!getNextTraceEntry(currentProcess->tracefile, &entry)) {
					state.freePagesRemaining +=
						 (unsigned long long)currentTable->numPages;
					state.finishedArray[processIndex] = 1;
					state.finishedCount++;
					flushCache(cache, currentTable);
					break;
				}

				memStatus = runVirtualMemorySimulation(
					 processes, processIndex, memResults, parameters->timeSlice,
					 memSimResults, &state, entry, numProcesses, &affectedPages,
					 cache);

				switch (memStatus) {
				case PROC_SKIP:
					continue;

				case PROC_FINISHED:
					flushCache(cache, currentTable);
					continue;

				case ERR:
					free(state.finishedArray);
					freeCache(cache);
					return 1;

				case SUCCESS:
					break;
				}

				PPN = currentTable->pages[affectedPages.addedIdx].phyAddr;
				pageOffset = entry.virAddr & PAGE_OFFSET_MASK;
				phyAddr = (PPN << PAGE_OFFSET_BITS) | pageOffset;

				runCacheSimulation(cache, cacheResults, cacheSimResults, phyAddr,
										 entry.operation, entry.instructionSize,
										 entry.isInstruction,
										 parameters->replacementPolicy,
										 parameters->blockSize);

				if (entry.instructionComplete) {
					instructionsExecutedThisSlice++;
				}
			}
		}
	}

	free(state.finishedArray);
	freeCache(cache);

	return 0;
}

int freeProcesses(int numFiles, Process **processes) {
	int i;

	for (i = 0; i < numFiles; i++) {
		freeProcessPageTable(processes[i]);
	}

	free(processes);

	return 0;
}
