#ifndef VM_RUNNER_H
#define VM_RUNNER_H

#include "cmd_parser.h"
#include "memory_calculations.h"

int runVMProcess(Parameters *parameters,
                 MemoryCalculationResults *mem_results);

#endif
