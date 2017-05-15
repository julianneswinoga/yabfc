#include "helpers.h"

/**
 * printf function wrapper to only print when there is verbose output
 * @param level	  Level of debug to print at
 * @param fmt     String format
 * @param VARARGS printf arguments
 */
void debugPrintf(/*int level, */const char *fmt, ...) {
	int level = 0;
	if (globalOptions.verbose >= level) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

/**
 * Function to find the starting index of a string from a null-terminated string character array
 * @param  stringData A null-terminated string character array
 * @param  index      Index of string to get
 * @return            Starting index of the requested string
 */
uint32_t stringIndexFromSectionIndex(uint8_t stringData[], uint8_t index) {
	uint32_t stringIndex = 0, i = 0; // Initialize variables
	for (; stringIndex != index; i++)
		if (*(stringData + i) == '\0') stringIndex++; // Increment the string index if we are on a null character

	return i; // Return the index
}

/**
 * Function that returns a filename without an extension on it
 * @param  fileName The full filename
 * @return 			The filename stripped of the extension
 */
char *filenameWithoutExtension(char *fileName) {
	char *ret;
	int   i, len;
	for (i = 0; fileName[i] != NULL && fileName[i] != '.'; i++)
		;
	len = i;
	ret = malloc(sizeof(char) * len);
	for (i = 0; i < len; i++) {
		ret[i] = fileName[i];
	}
	ret[i] = '\0';
	return ret;
}

/**
 * Function to add data to a section struct
 * @param section   The section to modify
 * @param startByte Byte array of data to add
 * @param size      How much data is being added
 */
void addSectionData(SECTION *section, uint8_t startByte[], uint32_t size) {
	(*section).bytes = realloc((*section).bytes, ((*section).size + size) * sizeof(uint8_t)); // Reallocate memory

	uint32_t byteIndex = 0;
	for (uint32_t i         = (*section).size; i < (*section).size + size; i++)
		(*section).bytes[i] = *(startByte + byteIndex++); // Add the data to the section

	(*section).size += size; // Increment the sections size by how much data was added
}

/**
 * Gets the relative position of a bracket
 * @param  instructions A pointer to an INSTRUCTIONS type to read characters from
 * @param  position     The position of the bracket we want to match
 * @return              The relative position of the bracket, -1 if not found
 */
int get_matching_bracket(INSTRUCTIONS *instructions, int position) {
	if (instructions->instruction[position].type != '[') {
		fprintf(stderr, "No bracket to match at position %i!\n", position);
		return -1;
	}

	int bracket_depth = 0;
	for (int i = position; i < instructions->size; i++) {
		if (instructions->instruction[i].type == '[') {
			bracket_depth++;
		} else if (instructions->instruction[i].type == ']') {
			bracket_depth--;
		}
		if (bracket_depth == 0 && instructions->instruction[i].type == ']') {
			return i - position;
		}
	}
	return -1; // Bracket not found
}
