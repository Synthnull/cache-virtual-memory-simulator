#include "cmd_parser.h"
#include <stdbool.h>

int main(int argc, char *argv[]) {
	Parameters *parameters = initParameters(3);
	bool err = parseCommandLine(argc, argv, parameters);

	if (err) {
		freeParameters(parameters);
		return 1;
	}
   
   //call functions

	freeParameters(parameters);

	return 0;
}
