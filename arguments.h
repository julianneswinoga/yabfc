#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <stdbool.h>

typedef struct globalOptionsStruct {
	int   numFiles;
	bool  verbose;
	bool  silent;
	char *outputFile;
} globalOptionsStruct;

extern globalOptionsStruct globalOptions;

#endif // ARGUMENTS_H_
