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
	FILE *     readFile, *writeFile;
	Elf32_Ehdr ELFHeader;

	ELFHeader.e_ident[EI_MAG0]       = 0x7f;
	ELFHeader.e_ident[EI_MAG1]       = 'E';
	ELFHeader.e_ident[EI_MAG2]       = 'L';
	ELFHeader.e_ident[EI_MAG3]       = 'F';
	ELFHeader.e_ident[EI_CLASS]      = ELFCLASS32;  // 32 bit ELF
	ELFHeader.e_ident[EI_DATA]       = ELFDATA2LSB; // Big endian
	ELFHeader.e_ident[EI_VERSION]    = EV_CURRENT;
	ELFHeader.e_ident[EI_OSABI]      = ELFOSABI_NONE; // OS independent
	ELFHeader.e_ident[EI_ABIVERSION] = 0x0;

	for (int i = EI_PAD; i < EI_NIDENT; i++) ELFHeader.e_ident[i] = 0x0; // zero padding

	ELFHeader.e_type    = ET_EXEC;   // Executable file
	ELFHeader.e_machine = EM_X86_64; // AMD x86-64
	ELFHeader.e_version = EV_CURRENT;
	ELFHeader.e_entry   = 0;                  // Entry point of program (>128 MB)
	ELFHeader.e_phoff   = sizeof(Elf32_Ehdr); // Program header table offset
	ELFHeader.e_shoff   = 0x0;                // Section header table offset
	ELFHeader.e_flags   = 0x0;                // Processor specific flags
	ELFHeader.e_ehsize  = sizeof(Elf32_Ehdr); // ELF Header size

	ELFHeader.e_phentsize = 56; // Size of each program header
	int e_phnum           = 0;  // Number of entries in program header table
	if (e_phnum > PN_XNUM) {
		ELFHeader.e_phnum = PN_XNUM;
		printf("WARNING: Elf32_Ehdr.e_phnum is greater than PN_XNUM, "
		       "sh_info should be set to the number of entries in the program header table");
	} else {
		ELFHeader.e_phnum = e_phnum;
	}

	ELFHeader.e_shentsize = 0; // Section header size, in bytes
	int e_shnum           = 0; // Number of entries in section header
	if (e_shnum > SHN_LORESERVE) {
		ELFHeader.e_shnum = 0x0;
		printf("WARNING: Elf32_Ehdr.e_shnum is greater than SHN_LORESERVE, "
		       "sh_size should be set to the number of entries in the program header table");
	} else {
		ELFHeader.e_shnum = e_shnum;
	}

	int e_shstrndx =
	    0; // Section header table index of the entry associated with the section name string table
	if (e_shstrndx > SHN_LORESERVE) {
		ELFHeader.e_shstrndx = SHN_XINDEX;
		printf("WARNING: Elf32_Ehdr.e_shstrndx is greater than SHN_LORESERVE, "
		       "sh_link should be set to the number of entries in the program header table");
	} else {
		ELFHeader.e_shstrndx = e_shstrndx;
	}

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	globalOptions.verbose    = arguments.verbose;
	globalOptions.outputFile = arguments.output_file;
	if (strcmp(globalOptions.outputFile, "") != 0) { // Output file specified
		if (globalOptions.numFiles > 1)
			printf("WARNING: Only compiling first file (output file specified)\n");
		globalOptions.numFiles = 1;
	}

	for (int i = 0; i < globalOptions.numFiles; i++) {
		debugPrintf("Opening file %s\n", arguments.args[i]);
		readFile  = fopen(arguments.args[i], "r");
		writeFile = fopen(
		    strcmp(globalOptions.outputFile, "") != 0 ? globalOptions.outputFile : "output1", "w+");

		printf("%i\n", ET_EXEC);

		fwrite(&ELFHeader, 1, sizeof(Elf32_Ehdr), writeFile); // Write header information

		debugPrintf("Done processing file %s\n", arguments.args[i]);
	}
	fclose(readFile);  // Close read file pointer
	fclose(writeFile); // Close write file pointer
}
