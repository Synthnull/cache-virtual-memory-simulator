#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_parser.h"

Parameters *initParameters(int fileCount) {
	Parameters *ptr = malloc(sizeof(Parameters));
	ptr->files.maxFiles = fileCount;
	ptr->files.numFiles = 0;
	return ptr;
}

int addFile(fileArray *files, char *fileName) {
	FILE *fptr = fopen(fileName, "r");

	if (fptr == NULL) {
		return 1;
	}

	files->files[files->numFiles].fileName = fileName;
   files->files[files->numFiles].filePtr = fptr;
	return 0;
}

Arguments *initArguments(int initialSize) {
	Arguments *ptr = malloc(sizeof(Arguments));
	ptr->numArguments = 0;
	ptr->capacity = initialSize;
	ptr->arguments = malloc(sizeof(char *) * ptr->capacity);

	return ptr;
}

int addArgument(Arguments *argumentList, char *str) {
	if (argumentList->numArguments == argumentList->capacity) {
		argumentList->capacity = argumentList->capacity * 2;
		argumentList->arguments = realloc(
			 argumentList->arguments, argumentList->capacity * sizeof(char *));
	}

	argumentList->numArguments++;
	argumentList->arguments[argumentList->numArguments - 1] = str;

	return 0;
}

int clearArguments(Arguments *argumentList) {
	int i;
	for (i = argumentList->numArguments - 1; i < 0; i--) {
	   argumentList->arguments[i] = NULL;
	}
	return 0;
}

bool parseCommandLine(int argc, char *argv[], Parameters *parameters) {
	int i;
	bool error;
	Arguments *currentArguments = initArguments(4);
	for (i = 0; i < argc; i++) {
		if (argv[i][0] == '-' && strlen(argv[i]) == 2) {
			while (argv[i][0] != '-') {
				addArgument(currentArguments, argv[i]);
				i++;
			}
			switch (argv[i][1]) {
			case 's':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}
				parameters->cacheSize = atoi(currentArguments->arguments[0]);

				if (parameters->cacheSize < 8 || parameters->cacheSize < 8192) {
					error = true;
				}
				break;
			case 'b':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->blockSize = atoi(currentArguments->arguments[0]);

				if (parameters->blockSize < 8 || parameters->blockSize < 64) {
					error = true;
				}

				break;
			case 'a':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}
				parameters->associativity = atoi(currentArguments->arguments[0]);
				break;
			case 'r':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				break;
			case 'p':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				break;
			case 'u':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				break;
			case 'n':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				break;
			case 'f':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				break;
			default:
				error = true;
				break;
			}
			clearArguments(currentArguments);
		}
	}

	return error;
}

