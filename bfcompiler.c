#include "bfcompiler.h"

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	/* Get the input argument from argp_parse, which we
	   know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
		case 'q':
		case 's': arguments->silent		 = 1; break;
		case 'v': arguments->verbose	 = 1; break;
		case 'o': arguments->output_file = arg; break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 2) /* Too many arguments. */
				argp_usage(state);

			arguments->args[state->arg_num] = arg;

			break;

		case ARGP_KEY_END:
			if (state->arg_num < 2) /* Not enough arguments. */
				argp_usage(state);
			break;

		default: return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	struct arguments arguments;

	/* Default values. */
	arguments.silent	  = 0;
	arguments.verbose	 = 0;
	arguments.output_file = "-";

	/* Parse our arguments; every option seen by parse_opt will
	   be reflected in arguments. */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	printf("ARG1 = %s\nARG2 = %s\nOUTPUT_FILE = %s\n"
		   "VERBOSE = %s\nSILENT = %s\n",
		   arguments.args[0], arguments.args[1], arguments.output_file,
		   arguments.verbose ? "yes" : "no", arguments.silent ? "yes" : "no");
}
