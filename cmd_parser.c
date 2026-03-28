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
	for (i = argumentList->numArguments - 1; i > 0; i--) {
	   argumentList->arguments[i] = NULL;
	}

   argumentList->numArguments = 0;

	return 0;
}

int checkPowerOfTwo(int x) {
    return x != 0 && (x & (x - 1)) == 0;
}

bool parseCommandLine(int argc, char *argv[], Parameters *parameters) {
	int i = 0;
   int j;
	bool error;
   char flag;
	Arguments *currentArguments = initArguments(4);
	while (i < argc) {
		if (argv[i][0] == '-' && strlen(argv[i]) == 2) {
         flag = argv[i][1];
         i++;
			while (i < argc && argv[i][0] != '-') {
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

				if ((parameters->cacheSize < 8 || parameters->cacheSize > 8192) || !checkPowerOfTwo(parameters->cacheSize)) {
					error = true;
				}
				break;
			case 'b':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

				parameters->blockSize = atoi(currentArguments->arguments[0]);

				if ((parameters->blockSize < 8 || parameters->blockSize > 64) || !checkPowerOfTwo(parameters->blockSize)) {
					error = true;
				}

				break;
			case 'a':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}
				parameters->associativity = atoi(currentArguments->arguments[0]);

            if(parameters->associativity != 1 && ((parameters->associativity < 2 || parameters->associativity > 16) || !checkPowerOfTwo(parameters->blockSize))) {
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

            if((parameters->physicalMemory < 2 || parameters->physicalMemory > 4096) || !checkPowerOfTwo(parameters->blockSize)) {
               error = true;
            }

				break;
			case 'u':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}
            
            parameters->physicalMemoryOS = atoi(currentArguments->arguments[0]);

            if(parameters->physicalMemoryOS >= 0 && parameters->physicalMemoryOS <= 100) {
               error = true;
            }
				break;
			case 'n':
				if (currentArguments->numArguments != 1) {
					error = true;
					break;
				}

            parameters->timeSlice = atoi(currentArguments->arguments[0]);

            if(parameters->timeSlice < -1) {
               error = true;
            }
            
				break;
			case 'f':
				if (currentArguments->numArguments < 1 || currentArguments->numArguments > 3) {
					error = true;
					break;
				}
            
            for (j = 0; j < currentArguments->numArguments; j++) {
               if(!addFile(&parameters->files, currentArguments->arguments[j])) {
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
	return error;
}

