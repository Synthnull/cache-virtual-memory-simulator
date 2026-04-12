#include "cache_calculations.h"
#include "cmd_parser.h"
#include "memory_calculations.h"
#include "print.h"
#include "virtual_memory_simulator.h"
#include "vm_runner.h"
#include <stdbool.h>

int main(int argc, char *argv[]) {
	Parameters *parameters = initParameters(3);
	bool err = parseCommandLine(argc, argv, parameters);

	if (err) {
		freeParameters(parameters);
		return 1;
	}

	MemoryCalculationResults memResults;
	CacheOutput cache_results;
	MemorySimulationResults memSimResults = {0};

	calculate_cache(parameters, &cache_results);
	calculate_memory(parameters->physicalMemory, parameters->physicalMemoryOS,
						  parameters->files.numFiles, &memResults);
	printCalculationResults(12, parameters, cache_results, memResults);
	runVMProcess(parameters, &memResults, &memSimResults);
   printVirMemorySimulationResults(memSimResults);
	freeParameters(parameters);
	return 0;
}
