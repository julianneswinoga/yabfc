#include "bfcompiler.h"

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct argumentStruct *arguments = state->input;

	switch (key) {
		case 'q':
		case 's': arguments->silent      = 1; break;
		case 'v': arguments->verbose     = 1; break;
		case 'o': arguments->output_file = arg; break;

		case ARGP_KEY_ARG:
			arguments->args[state->arg_num] = arg;
			globalOptions.numFiles++;
			break;

		case ARGP_KEY_END:
			if (state->arg_num < 1) /* Not enough files */
				argp_usage(state);
			break;

		default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	FILE *readFile, *writeFile;
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	globalOptions.verbose    = arguments.verbose;
	globalOptions.outputFile = arguments.output_file;
	if (strcmp(globalOptions.outputFile, "") != 0) { // Output file specified
		if (globalOptions.numFiles > 1)
			printf(
			    "WARNING: Only compiling first file (output file specified)\n");
		globalOptions.numFiles = 1;
	}

	for (int i = 0; i < globalOptions.numFiles; i++) {
		debugPrintf("Opening file %s\n", arguments.args[i]);
		readFile  = fopen(arguments.args[i], "r");
		writeFile = fopen(strcmp(globalOptions.outputFile, "") != 0
		                      ? globalOptions.outputFile
		                      : "output1",
		                  "w+");

		uint8_t m[2] = {5, 10};
		fwrite(m, sizeof(m[0]), sizeof(m), writeFile);
	}
	fclose(readFile);  // Close read file pointer
	fclose(writeFile); // Close write file pointer
}
