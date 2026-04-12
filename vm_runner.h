#ifndef VM_RUNNER_H
#define VM_RUNNER_H

#include "cmd_parser.h"
#include "memory_calculations.h"
#include "page_table.h"
#include "virtual_memory_simulator.h"

int runVMProcess(Parameters *parameters, MemoryCalculationResults *memResults,
					  MemorySimulationResults *memSimResults, Process **Processes);

int freeProcesses(int numFiles, Process **processes);

#endif
