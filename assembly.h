#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arguments.h"
#include "helpers.h"
#include "structures.h"

#define SIZEOFSTACKADDRESS (8)

// Function declarations
void construct_ADD(CODE *code, uint32_t add);
void construct_SUB(CODE *code, uint32_t sub);
void construct_ADDESP(CODE *code, uint32_t num);
void construct_SUBESP(CODE *code, uint32_t num);
void construct_LPSTART(CODE *code);
void construct_LPEND(CODE *code);
void construct_INPUT(CODE *code);
void construct_PRINT(CODE *code);
void construct_START(CODE *code);
void construct_END(CODE *code);
void construct_CLEAR(CODE *code);
void construct_MULTIPLY(CODE *code, int a, int offset);

#endif  // ASSEMBLY_H_
