#include <stdbool.h>
#include "cmd_parser.h"

int main(int argc, char *argv[]) {
	Parameters *parameters = initParameters(3);
	if (parseCommandLine(argc, argv, parameters)) {
		return 1;
	}
	return 0;
}
