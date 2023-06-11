#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "arguments.h"
#include "assembly.h"
#include "elfHelper.h"
#include "structures.h"

// Function declarations
void debugPrintf(int level, const char *fmt, ...);
uint32_t stringIndexFromSectionIndex(const uint8_t *const stringData, uint8_t index);
char *filenameWithoutExtension(const char *const fileName);
int getMatchingBracket(INSTRUCTIONS *instructions, int position, bool throwError);

#endif  // HELPERS_H_
