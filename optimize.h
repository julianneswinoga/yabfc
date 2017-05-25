#ifndef OPTIMIZE_H_
#define OPTIMIZE_H_

#include "arguments.h"
#include "assembly.h"
#include "structures.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int total_ADDSUB_compress;
extern int total_PTR_compress;
extern int total_CLEARLOOP;
extern int total_MULTIPLY;

bool optimize_compress_ADDSUB(INSTRUCTIONS *, int *, CODE *);
bool optimize_compress_PTR(INSTRUCTIONS *, int *, CODE *);
bool optimize_clear_loop(INSTRUCTIONS *, int *, CODE *);
bool optimize_multiplication(INSTRUCTIONS *, int *, CODE *);

#endif // OPTIMIZE_H_
