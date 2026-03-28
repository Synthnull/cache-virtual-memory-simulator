#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include <stdio.h>
#include <stdbool.h>
typedef enum {
	RR,
	RND,
} ReplacementPolicy;

typedef struct {
	int numArguments;
	int capacity;
	char **arguments;
} Arguments;

typedef struct {
	char *fileName;
	FILE *filePtr;
} File;

typedef struct {
	File *files;
	int numFiles;
	int maxFiles;
} fileArray;

typedef struct {
	int cacheSize;
	int blockSize;
	int associativity;
	ReplacementPolicy replacementPolicy;
	int physicalMemory;
	int physicalMemoryOS;
	int timeSlice;
	fileArray files;
} Parameters;

Parameters *initParameters(int fileCount);
bool parseCommandLine(int argc, char *argv[], Parameters *parameters);

#endif
