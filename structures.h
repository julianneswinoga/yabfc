#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <stdint.h>

typedef struct {
    int size;
    uint8_t *bytes;
} CODE;

typedef struct {
    char type;
    int bracketMatch;
} INSTRUCTION;

typedef struct {
    uint32_t size;
    INSTRUCTION *instruction;
} INSTRUCTIONS;

#endif  // STRUCTURES_H_
