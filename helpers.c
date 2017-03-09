#include "helpers.h"

/**
 * printf function wrapper to only print when there is verbose output
 * @param fmt     String format
 * @param VARARGS printf arguments
 */
void debugPrintf(const char *fmt, ...) {
	if (globalOptions.verbose) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

uint32_t stringIndexFromSectionIndex(uint8_t stringData[], uint8_t index) {
	uint32_t stringIndex = 0;
	uint16_t i;
	for (i = 0; stringIndex != index; i++)
		if (*(stringData + i) == '\0') stringIndex++;
	return i;
}

void addSectionData(SECTION *section, uint8_t startByte[], uint16_t size) {
	(*section).bytes = realloc((*section).bytes, ((*section).size + size) * sizeof(uint8_t));

	uint16_t byteIndex = 0;
	for (uint16_t i = (*section).size; i < (*section).size + size; i++) (*section).bytes[i] = *(startByte + byteIndex++);

	(*section).size += size;
}
