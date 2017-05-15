#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_

#include "arguments.h"
#include "helpers.h"
#include "structures.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function declarations
void construct_arbitrary(CODE *, uint8_t *, uint16_t);
void construct_ADD(CODE *, uint32_t);
void construct_SUB(CODE *, uint32_t);
void construct_ADDESP(CODE *, uint32_t);
void construct_SUBESP(CODE *, uint32_t);
void construct_LPSTART(CODE *);
void construct_LPEND(CODE *);
void construct_INPUT(CODE *);
void construct_PRINT(CODE *);
void construct_START(CODE *);
void construct_END(CODE *);

#endif // ASSEMBLY_H_
