#include "optimize.h"

int total_ADDSUB_compress = 0;
int total_PTR_compress    = 0;
int total_CLEARLOOP       = 0;
int total_MULTIPLY        = 0;

/**
 * Compresses superfluous instructions into a single positive or negative number
 * @param  instructions A pointer to an INSTRUCTIONS type to read characters from
 * @param  position     A pointer to the current position being read
 * @param  inc          What character specifies a `+1`
 * @param  dec          What character specifies a `-1`
 * @return              A number that represents the compressed instruction string
 */
int lookahead_compress(INSTRUCTIONS *instructions, int *position, char inc, char dec) {
	int count = 0;
	while ((*position) < instructions->size &&
	       (instructions->instruction[(*position)].type == inc ||
	        instructions->instruction[(*position)].type == dec)) { // While we are less than the array size and we are reading valid characters
		if (instructions->instruction[(*position)].type == inc) {
			count++;
		} else {
			count--;
		}
		(*position)++;
	}
	if (count != 0) {
		(*position)--; // Decrement to get us back to the last character we should be on
	}
	return count;
}

bool optimize_compress_ADDSUB(INSTRUCTIONS *instructions, int *position, CODE *code) {
	if (globalOptions.optimize < 1) // Only do on optimization level 1 and higher
		return false;

	int count = 0;
	if ((count = lookahead_compress(instructions, position, '+', '-')) == 0) {
		return false;
	} else {
		total_ADDSUB_compress += count < 0 ? -count : count;
		if (count > 0) {
			construct_ADD(code, count);
		} else if (count < 0) {
			construct_SUB(code, -count);
		}
		return true;
	}
}

bool optimize_compress_PTR(INSTRUCTIONS *instructions, int *position, CODE *code) {
	if (globalOptions.optimize < 1) // Only do on optimization level 1 and higher
		return false;

	int count = 0;
	if ((count = lookahead_compress(instructions, position, '<', '>')) == 0) {
		return false;
	} else {
		total_PTR_compress += count < 0 ? -count : count;
		if (count > 0) {
			construct_ADDESP(code, count * SIZEOFSTACKADDRESS);
		} else if (count < 0) {
			construct_SUBESP(code, -count * SIZEOFSTACKADDRESS);
		}
		return true;
	}
}

bool optimize_clear_loop(INSTRUCTIONS *instructions, int *position, CODE *code) {
	if (globalOptions.optimize < 2) // Only do on optimization level 2 and higher
		return false;

	char clearLoop[] = "[-]";
	int  i;
	for (i = 0; i < sizeof(clearLoop) - 1; i++) {
		if (instructions->instruction[i + (*position)].type != clearLoop[i])
			return false;
	}

	construct_CLEAR(code);
	(*position) += sizeof(clearLoop) - 2;
	total_CLEARLOOP += 1;

	return true;
}

bool optimize_multiplication(INSTRUCTIONS *instructions, int *position, CODE *code) {
	if (globalOptions.optimize < 3) // Only do on optimization level 3 and higher
		return false;

	int startbracket = (*position), endbracket;

	if ((endbracket = get_matching_bracket(instructions, startbracket, false)) == -1) { // Check we are in a 'leaf' loop
		return false;
	}
	endbracket += startbracket; // Turn a relative offset into an absolute number

	uint16_t memsize = 1;
	int *    mem     = malloc(memsize * sizeof(int));
	mem[0]           = 0;
	int p            = 0;
	for (int i = startbracket + 1; i < endbracket; i++) {
		if (strchr("><+-", instructions->instruction[i].type) == NULL) { // Check that the loop only contains arithmetic instructions
			free(mem);
			return false;
		}
		switch (instructions->instruction[i].type) { // Interpret the loop
			case '+':
			case '-':
				mem[p] = mem[p] + lookahead_compress(instructions, &i, '+', '-');
				break;
			case '>':
			case '<':
				p += lookahead_compress(instructions, &i, '>', '<');
				if (p < 0) {
					free(mem);
					return false;
				}
				if (p > memsize - 1) {
					mem = (int *)realloc(mem, (p + 1) * sizeof(int));
					for (int j = memsize; j < p + 1; j++) {
						mem[j] = 0; // Set newly allocated memory to 0
					}
					memsize = p + 1;
				}
				break;
		}
	}

	if (p != 0 || mem[0] != -1) {
		free(mem);
		return false;
	}

	debugPrintf(2, "Multiply of");
	for (int i = 1; i < memsize; i++) {
		if (mem[i] == 0) {
			debugPrintf(2, " %s", "NUL");
		} else {
			debugPrintf(2, " %i", mem[i]);
			construct_MULTIPLY(code, mem[i], i);
			total_MULTIPLY += 1;
		}
	}
	debugPrintf(2, "\n");
	construct_CLEAR(code); // Clear the initial pointer

	(*position) += endbracket - startbracket; // Move to the next instruction
	free(mem);
	return true;
}
