#ifndef OPTIMIZE_H_
#define OPTIMIZE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments.h"
#include "assembly.h"
#include "structures.h"

extern int total_ADDSUB_compress;
extern int total_PTR_compress;
extern int total_CLEARLOOP;
extern int total_MULTIPLY;

bool optimize_compress_ADDSUB(INSTRUCTIONS *instructions, int *position, CODE *code);
bool optimize_compress_PTR(INSTRUCTIONS *instructions, int *position, CODE *code);
bool optimize_clear_loop(INSTRUCTIONS *instructions, int *position, CODE *code);
bool optimize_multiplication(INSTRUCTIONS *instructions, int *position, CODE *code);

#endif  // OPTIMIZE_H_
