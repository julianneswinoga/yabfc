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
	if (globalOptions.optimize < 3) // Only do on optimization level 2 and higher
		return false;

	char multiply[] = "[->\x01>\x02<<]";
	int  a = 0, b = 0, totalCharacters = sizeof(multiply) - 1, current = (*position);
	int  i, before;
	for (i = 0; i < sizeof(multiply) - 1; i++) {
		if (multiply[i] == '\x01') {
			before = current;
			a      = lookahead_compress(instructions, &current, '+', '-');
			totalCharacters += current - before;
			current++;
			continue;
		} else if (multiply[i] == '\x02') {
			before = current;
			b      = lookahead_compress(instructions, &current, '+', '-');
			totalCharacters += current - before;
			current++;
			continue;
		}
		if (instructions->instruction[current].type != multiply[i])
			return false;
		current++;
	}

	debugPrintf(3, "Multiply of %i, %i\n", a, b);
	construct_MULTIPLY(code, a, b);
	(*position) += totalCharacters - 1;
	total_MULTIPLY += 1;

	return true;
}
