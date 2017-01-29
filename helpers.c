#include "helpers.h"

void debugPrintf(const char *fmt, ...) {
	if (globalOptions.verbose) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}
