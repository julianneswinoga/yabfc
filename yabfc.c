#include "yabfc.h"

/**
 * Function which takes care of parsing command line switches
 * @param  key   Argument switch
 * @param  arg   Argument data string
 * @param  state The state of the current arguments
 * @return       error_t on error
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct argumentStruct *arguments = state->input;

	switch (key) { // Switch for setting the argument values
		case 'q':
		case 's': arguments->silent      = 1; break;
		case 'v': arguments->verbose     = 1; break;
		case 'o': arguments->output_file = arg; break;

		case ARGP_KEY_ARG:
			arguments->args[state->arg_num] = arg;
			globalOptions.numFiles++;
			break;

		case ARGP_KEY_END:
			if (state->arg_num < 1) argp_usage(state); // Not enough files
			break;

		default:
			return ARGP_ERR_UNKNOWN; // Error
	}
	return 0; // No error
}

int main(int argc, char *argv[]) {
	FILE *readFile, *writeFile;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	globalOptions.verbose    = arguments.verbose;
	globalOptions.outputFile = arguments.output_file;
	if (strcmp(globalOptions.outputFile, "") != 0) { // Output file specified
		if (globalOptions.numFiles > 1) printf("WARNING: Only compiling first file (output file specified)\n");
		globalOptions.numFiles = 1;
	}

	for (int i = 0; i < globalOptions.numFiles; i++) {
		debugPrintf("Opening file %s\n", arguments.args[i]);
		readFile  = fopen(arguments.args[i], "r");
		writeFile = fopen(strcmp(globalOptions.outputFile, "") != 0 ? globalOptions.outputFile : "output1", "w+");

		SECTION text, data, stringTable;
		text.size         = 0;
		text.bytes        = malloc(text.size * sizeof(uint8_t));
		data.size         = 0;
		data.bytes        = malloc(data.size * sizeof(uint8_t));
		stringTable.size  = 0;
		stringTable.bytes = malloc(stringTable.size * sizeof(uint8_t));

		Elf64_Ehdr ELFHeader;
		setupELFHeader(&ELFHeader, ENTRY_POINT, PGM_HEADER_TBL_LOC, 0x0, PGM_HEADER_SIZE, PGM_HEADER_NUM, SEC_HEADER_SIZE, SEC_HEADER_NUM);

		debugPrintf("Constructing .text section\n");
		uint8_t code[] = {0x31, 0xFF, 0xBF, 0x2A, 0x00, 0x00, 0x00, 0xB8, 0x3C, 0x00, 0x00, 0x00, 0x0F, 0x05};
		addSectionData(&text, (uint8_t *)&code, sizeof(code));

		debugPrintf("Constructing .data section\n");
		uint8_t tempData = '0';
		for (int i = 0; i < 21 - 3; i++) addSectionData(&data, (uint8_t *)&tempData, sizeof(tempData));

		debugPrintf("Constructing .shrtrab section\n");
		uint8_t stringData[] = "\0.text\0.data\0.shrtrab\0";
		addSectionData(&stringTable, (uint8_t *)&stringData, sizeof(stringData));

		Elf64_Phdr programHeaderTable[PGM_HEADER_NUM];
		setupprogramHeader(&programHeaderTable[0], PF_R + PF_X, TEXT_FILE_LOC, TEXT_MEM_LOC, text.size);
		setupprogramHeader(&programHeaderTable[1], PF_R + PF_W + PF_X, TEXT_FILE_LOC + text.size, TEXT_MEM_LOC + text.size, data.size);

		Elf64_Shdr sectionHeaderTable[SEC_HEADER_NUM];
		setupSectionHeader(&sectionHeaderTable[0], 0, SHT_NULL, 0, 0, 0, 0);
		setupSectionHeader(&sectionHeaderTable[1], stringIndexFromSectionIndex(stringTable.bytes, 1), SHT_PROGBITS, SHF_ALLOC + SHF_EXECINSTR, TEXT_MEM_LOC, TEXT_FILE_LOC, text.size);
		setupSectionHeader(&sectionHeaderTable[2], stringIndexFromSectionIndex(stringTable.bytes, 2), SHT_PROGBITS, SHF_ALLOC + SHF_WRITE, TEXT_MEM_LOC + text.size, TEXT_FILE_LOC + text.size,
		                   data.size);
		setupSectionHeader(&sectionHeaderTable[3], stringIndexFromSectionIndex(stringTable.bytes, 3), SHT_STRTAB, 0, 0, TEXT_FILE_LOC + text.size + data.size, stringTable.size);
		ELFHeader.e_shstrndx = 3;

		ELFHeader.e_shoff = SEC_HEADER_NUM == 0 ? 0x0 : TEXT_FILE_LOC + text.size + data.size + stringTable.size;

		debugPrintf("Writing ELF header\n");
		fwrite(&ELFHeader, 1, sizeof(ELFHeader), writeFile);

		debugPrintf("Writing program header table\n");
		fwrite(&programHeaderTable, 1, sizeof(programHeaderTable), writeFile);

		debugPrintf("Writing .text section of size %i bytes\n", text.size);
		fwrite(text.bytes, 1, text.size, writeFile);
		debugPrintf("Writing .data section of size %i bytes\n", data.size);
		fwrite(data.bytes, 1, data.size, writeFile);
		debugPrintf("Writing .shrtrab section of size %i bytes\n", stringTable.size);
		fwrite(stringTable.bytes, 1, stringTable.size, writeFile);

		debugPrintf("Writing section header, %i sections\n", ELFHeader.e_shnum);
		fwrite(&sectionHeaderTable, 1, sizeof(sectionHeaderTable), writeFile);

		debugPrintf("Done processing file %s\n", arguments.args[i]);
	}
	fclose(readFile);
	fclose(writeFile);
}
