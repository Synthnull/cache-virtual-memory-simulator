#include <stdlib.h>

#include "page_table.h"
#include "virtual_memory_simulator.h"
#include "vm_runner.h"

int runVMProcess(Parameters *parameters, MemoryCalculationResults *memResults,
					  MemorySimulationResults *memSimResults, Process **processes) {

	int i;

	processes = malloc(sizeof(Process *) * parameters->files.numFiles);
	if (processes == NULL) {
		return 1;
	}

	for (i = 0; i < parameters->files.numFiles; i++) {
		processes[i] =
			 InitProcessPageTable(10, (int)memResults->number_physical_pages,
										 parameters->files.files[i].filePtr, parameters->files.files[i].fileName);

		if (processes[i] == NULL) {
			while (--i >= 0) {
				freeProcessPageTable(processes[i]);
			}
			free(processes);
			return 1;
		}
	}

	runVirtualMemorySimulation(processes, memResults, parameters->timeSlice,
										memSimResults);

	for (i = 0; i < parameters->files.numFiles; i++) {
		freeProcessPageTable(processes[i]);
	}

	free(processes);

	return 0;
}
