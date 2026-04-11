#include <stdlib.h>

#include "vm_runner.h"
#include "page_table.h"
#include "virtual_memory_simulator.h"

int runVMProcess(Parameters *parameters,
                 MemoryCalculationResults *mem_results) {

    Process **processes;
    int i;

    processes = malloc(sizeof(Process *) * parameters->files.numFiles);
    if (processes == NULL) {
        return 1;
    }

    for (i = 0; i < parameters->files.numFiles; i++) {
        processes[i] = InitProcessPageTable(
            10,
            (int)mem_results->number_physical_pages,
            parameters->files.files[i].filePtr
        );

        if (processes[i] == NULL) {
            while (--i >= 0) {
                freeProcessPageTable(processes[i]);
            }
            free(processes);
            return 1;
        }
    }

    runVirtualMemorySimulation(processes, mem_results);

    for (i = 0; i < parameters->files.numFiles; i++) {
        freeProcessPageTable(processes[i]);
    }

    free(processes);

    return 0;
}
