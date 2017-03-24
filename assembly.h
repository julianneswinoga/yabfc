#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_

#include "arguments.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
	int      size;
	uint8_t *bytes;
} CODE;

// Function declarations
void construct_arbitrary(CODE *, uint8_t *, uint16_t);
void construct_INC(CODE *);
void construct_DEC();
void construct_ADDESP();
void construct_SUBESP();
void construct_LPSTART();
void construct_LPEND();
void construct_INPUT();
void construct_PRINT(CODE *);
void construct_END(CODE *);

/*
+	INC [esp] (FF-0 r/m16/32/64)
-	DEC [esp] (FF-1 r/m16/32/64)
>	ADD 4, esp (01-r r/m16/32/64 r16/32/64)
<	SUB 4, esp (29-4 r/m16/32/64 r16/32/64)
[
]
,
.
*/

#endif // ASSEMBLY_H_
