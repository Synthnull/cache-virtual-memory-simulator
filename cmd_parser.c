#include "cmd_parser.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parameters *initParameters(int fileCount) {
	Parameters *ptr = malloc(sizeof(Parameters));
	ptr->files.maxFiles = fileCount;
	ptr->files.numFiles = 0;
	ptr->files.files = calloc(fileCount, sizeof(File));
	return ptr;
}

int addFile(fileArray *files, char *fileName) {
	FILE *fptr = fopen(fileName, "r");

	if (fptr == NULL) {
		return 1;
	}

	files->numFiles++;

	if (files->maxFiles < files->numFiles) {
		return 1;
	}

	files->files[files->numFiles - 1].fileName = fileName;
	files->files[files->numFiles - 1].filePtr = fptr;
	return 0;
}

int closeFiles(fileArray *ptr) {
	int i;
	for (i = 0; i < ptr->numFiles; i++) {
		if (ptr->files[i].filePtr != NULL) {
			fclose(ptr->files[i].filePtr);
			ptr->files[i].fileName = NULL;
			ptr->files[i].filePtr = NULL;
		}
	}

	free(ptr->files);
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
	for (i = argumentList->numArguments - 1; i >= 0; i--) {
		argumentList->arguments[i] = NULL;
	}

	argumentList->numArguments = 0;

	return 0;
}

int freeArguments(Arguments *ptr) {
	clearArguments(ptr);
	free(ptr->arguments);
	free(ptr);
	return 0;
}

int checkPowerOfTwo(int x) { return x != 0 && (x & (x - 1)) == 0; }

bool isFlag(char *str) {
	return str[0] == '-' && isalpha((unsigned char)str[1]);
}

bool parseCommandLine(int argc, char *argv[], Parameters *parameters) {
	int i = 1;
	int j;
	bool error = false;
	char flag;
	Arguments *currentArguments = initArguments(4);
	while (i < argc) {
		if (isFlag(argv[i])) {

			flag = argv[i][1];
			i++;
			while (i < argc) {
				if (isFlag(argv[i])) {
					break;
				}
				addArgument(currentArguments, argv[i]);
				i++;
			}

			switch (flag) {
			case 's':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}
				parameters->cacheSize = atoi(currentArguments->arguments[0]);

				if ((parameters->cacheSize < 8 || parameters->cacheSize > 8192) ||
					 !checkPowerOfTwo(parameters->cacheSize)) {
					printf("%s\n", "ERROR: Invalid Cache Size");
					error = true;
				}
				break;
			case 'b':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->blockSize = atoi(currentArguments->arguments[0]);

				if ((parameters->blockSize < 8 || parameters->blockSize > 64) ||
					 !checkPowerOfTwo(parameters->blockSize)) {
					printf("%s\n", "ERROR: Invalid Block Size");

					error = true;
				}

				break;
			case 'a':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}
				parameters->associativity = atoi(currentArguments->arguments[0]);

				if (parameters->associativity != 1 &&
					 ((parameters->associativity < 2 ||
						parameters->associativity > 16) ||
					  !checkPowerOfTwo(parameters->blockSize))) {

printf("%s\n", "ERROR: Invalid Associativity");
					error = true;
				}
				break;
			case 'r':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->replacementPolicy = RR;

				break;
			case 'p':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->physicalMemory = atoi(currentArguments->arguments[0]);

				if ((parameters->physicalMemory < 2 ||
					  parameters->physicalMemory > 4096) ||
					 !checkPowerOfTwo(parameters->blockSize)) {
printf("%s\n", "ERROR: Invalid Physical Memory Size");
					error = true;
				}

				break;
			case 'u':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->physicalMemoryOS = atoi(currentArguments->arguments[0]);

				if (parameters->physicalMemoryOS < 0 ||
					 parameters->physicalMemoryOS > 100) {
printf("%s\n", "ERROR: Invalid OS Memory Utilization");
					error = true;
				}
				break;
			case 'n':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->timeSlice = atoi(currentArguments->arguments[0]);

				if (parameters->timeSlice < -1) {
					error = true;
				}

				break;
			case 'f':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				for (j = 0; j < currentArguments->numArguments; j++) {
					if (addFile(&parameters->files,
									currentArguments->arguments[j])) {
printf("%s\n", "ERROR: Invalid File");
						error = true;
					}
				}
				break;
			default:
				error = true;
				break;
			}
			clearArguments(currentArguments);
		} else {
			i++;
		}
	}
	freeArguments(currentArguments);
	return error;
}

int freeParameters(Parameters *ptr) {
	closeFiles(&ptr->files);
	free(ptr);
	return 0;
}
