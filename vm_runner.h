#ifndef VM_RUNNER_H
#define VM_RUNNER_H

#include "cmd_parser.h"
#include "memory_calculations.h"
#include "virtual_memory_simulator.h"

int runVMProcess(Parameters *parameters, MemoryCalculationResults *memResults,
					  MemorySimulationResults *memSimResults);

#endif
