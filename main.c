#include <stdbool.h>
#include "cmd_parser.h"
#include "memory_calculations.h"
#include "cache_calculations.h"
#include "print.h"
#include "vm_runner.h"

int main(int argc, char *argv[]) {
	Parameters *parameters = initParameters(3);
	bool err = parseCommandLine(argc, argv, parameters);

	if (err) {
		freeParameters(parameters);
		return 1;
	}
   
   MemoryCalculationResults mem_results;
   CacheOutput cache_results;

   calculate_cache(parameters, &cache_results);
   calculate_memory(parameters->physicalMemory, parameters->physicalMemoryOS, parameters->files.numFiles, &mem_results);
   printCalculationResults(12, parameters, cache_results, mem_results);
	runVMProcess(parameters, &mem_results);
	freeParameters(parameters);
	return 0;

}
