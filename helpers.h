#ifndef HELPERS_H_
#define HELPERS_H_

#include "arguments.h"
#include "elfHelper.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Function declarations
void     debugPrintf(const char *, ...);
uint32_t stringIndexFromSectionIndex(uint8_t *, uint8_t);
void     addSectionData(SECTION *, uint8_t *, uint16_t);

#endif // HELPERS_H_
