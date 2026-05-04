#include "cache_calculations.h"
#include "cache_simulator.h"
#include "cmd_parser.h"
#include "memory_calculations.h"
#include "page_table.h"
#include "print.h"
#include "sim_runner.h"
#include "virtual_memory_simulator.h"
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	Parameters *parameters = initParameters(3);
	bool err = parseCommandLine(argc, argv, parameters);

	if (err) {
		freeParameters(parameters);
		return 1;
	}

	printf("numfiles:%d", parameters->files.numFiles);

	MemoryCalculationResults memResults = {0};
	CacheOutput cache_results = {0};
	MemorySimulationResults memSimResults = {0};
	CacheSimulationResults cacheSimResults = {0};

	calculate_cache(parameters, &cache_results);
	calculate_memory(parameters->physicalMemory, parameters->physicalMemoryOS,
						  parameters->files.numFiles, &memResults);
	printCalculationResults(12, parameters, cache_results, memResults);
	Process **processes =
		 calloc(parameters->files.numFiles + 1, sizeof(Process *));
	runSimulation(parameters, &memResults, parameters->associativity, &cache_results, &memSimResults,
					  &cacheSimResults, processes);
	printVirMemorySimulationResults(memSimResults, processes,
											  parameters->files.numFiles);
   printCacheSimulationResults(cacheSimResults, cache_results);
	freeProcesses(parameters->files.numFiles, processes);
	freeParameters(parameters);
	return 0;
}
