#ifndef ASSEMBLY_H_
#define ASSEMBLY_H_

#include "arguments.h"

// Function declarations
void construct_INC();

/*
+	INC (FF-0 r/m16/32/64)
-	DEC (FF-1 r/m16/32/64)
<	ADD 4, esp (01-r r/m16/32/64 r16/32/64)
>	SUB 4, esp (29-4 r/m16/32/64 r16/32/64)
[
]
,
.
*/

#endif // ASSEMBLY_H_