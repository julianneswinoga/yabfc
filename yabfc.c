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
			return i;
		}
	}
	return -1; // Bracket not found
}

/**
 * Main function
 * make ; and ./yabfc -v hello.bf ; and readelf output1 -a ; and hexdump -v -C output1 ; and ./output1
 * @param  argc Argument count
 * @param  argv arguments
 * @return      Program return value
 */
int main(int argc, char *argv[]) {
	FILE *readFile, *writeFile; // Read and write file pointers

	argp_parse(&argp, argc, argv, 0, 0, &arguments); // Parse the command line arguments
	globalOptions.verbose    = arguments.verbose;    // Setting global options
	globalOptions.outputFile = arguments.output_file;
	if (strcmp(globalOptions.outputFile, "") != 0) { // Output file specified
		if (globalOptions.numFiles > 1) printf("WARNING: Only compiling first file (output file specified)\n");
		globalOptions.numFiles = 1;
	}

	for (int i = 0; i < globalOptions.numFiles; i++) { // Loopint through the input files
		debugPrintf("Opening file %s\n", arguments.args[i]);
		readFile  = fopen(arguments.args[i], "r"); // Open file for reading
		writeFile = fopen(strcmp(globalOptions.outputFile, "") != 0 ? globalOptions.outputFile : "output1", "w+");

		SECTION text, data, stringTable; // Set up sections
		text.size         = 0;
		text.bytes        = malloc(text.size * sizeof(uint8_t));
		data.size         = 0;
		data.bytes        = malloc(data.size * sizeof(uint8_t));
		stringTable.size  = 0;
		stringTable.bytes = malloc(stringTable.size * sizeof(uint8_t));

		Elf64_Ehdr ELFHeader; // Initialize the ELF header
		setupELFHeader(&ELFHeader,
		               ENTRY_POINT,
		               PGM_HEADER_TBL_LOC,
		               0x0, /*Section header location*/
		               PGM_HEADER_SIZE, PGM_HEADER_NUM,
		               SEC_HEADER_SIZE, SEC_HEADER_NUM); // Mostly set up the ELF header

		debugPrintf("Constructing .text section\n");

		INSTRUCTIONS instructions = {
		    .size        = 0,
		    .instruction = malloc(0)};

		debugPrintf("Program as read from file: ");
		char readCharacter = 0;
		while ((readCharacter = fgetc(readFile)) != EOF) {
			if (strpbrk(VALID_COMMANDS, &readCharacter) != NULL) {
				debugPrintf("%c", readCharacter);
				INSTRUCTION tempInstruction = {
				    .type         = readCharacter,
				    .bracketMatch = -1};

				instructions.instruction                      = (INSTRUCTION *)realloc(instructions.instruction, (instructions.size + 1) * sizeof(INSTRUCTION));
				instructions.instruction[instructions.size++] = tempInstruction;
			}
		}
		debugPrintf("\n");

		CODE code = {
		    .size  = 0,
		    .bytes = malloc(0)};

		/*
xor ebp, ebp
mov r9, rdx
pop rsi
mov rdx, rsp
and rsp, 0xfffffffffffffff0
sub rsp, 4
 */
		uint8_t preamble[] = {0x31, 0xED, 0x49, 0x89, 0xD1, 0x5E, 0x48, 0x89, 0xE2, 0x48, 0x83, 0xE4, 0xF0, 0x48, 0x83, 0xEC, 0x04}; // Preamble
		construct_arbitrary(&code, preamble, sizeof(preamble));

		int relativeBracket;
		for (int i = 0; i < instructions.size; i++) {
			switch (instructions.instruction[i].type) {
				case '+':
					construct_INC(&code);
					break;
				case '-':
					construct_DEC(&code);
					break;
				case '>':
					construct_ADDESP(&code);
					break;
				case '<':
					construct_SUBESP(&code);
					break;
				case '[':
					if ((relativeBracket = get_matching_bracket(&instructions, i)) == -1) {
						fprintf(stderr, "Opening bracket does not have a matching closing bracket at position %i!\n", i);
						exit(1);
					}
					instructions.instruction[i].bracketMatch                   = relativeBracket;
					instructions.instruction[i + relativeBracket].bracketMatch = -relativeBracket;
					construct_LPSTART(&code);
					break;
				case ']':
					if (instructions.instruction[i].bracketMatch == -1) {
						fprintf(stderr, "Closing bracket does not have a matching opening bracket at position %i!\n", i);
						exit(1);
					}
					construct_LPEND(&code);
					break;
				case ',':
					construct_INPUT(&code);
					break;
				case '.':
					construct_PRINT(&code);
					break;
			}
		}
		construct_END(&code);

		addSectionData(&text, code.bytes, code.size); // Add some example code

		debugPrintf("Constructing .data section\n");
		uint8_t tempData = '0';
		for (int i = 0; i < 21 - 3; i++)
			addSectionData(&data, (uint8_t *)&tempData, sizeof(tempData)); // Add some example data

		debugPrintf("Constructing .shrtrab section\n");
		uint8_t stringData[] = "\0.text\0.data\0.shrtrab\0"; // Set up the string table section with the appropriate names
		addSectionData(&stringTable, (uint8_t *)&stringData, sizeof(stringData));

		Elf64_Phdr programHeaderTable[PGM_HEADER_NUM];
		setupprogramHeader(&programHeaderTable[0], /* .text segment */
		                   PF_R + PF_X,            /* Segment permissions */
		                   TEXT_FILE_LOC,          /* File offset for the contents of the segment */
		                   TEXT_MEM_LOC,           /* Virtual address where the segment will be loaded */
		                   text.size               /* Segment size */
		                   );
		setupprogramHeader(&programHeaderTable[1], /* .data segment */
		                   PF_R + PF_W + PF_X,
		                   TEXT_FILE_LOC + text.size,
		                   TEXT_MEM_LOC + text.size,
		                   data.size);

		Elf64_Shdr sectionHeaderTable[SEC_HEADER_NUM];
		setupSectionHeader(&sectionHeaderTable[0], 0, SHT_NULL, 0, 0, 0, 0); // Set up null section header

		setupSectionHeader(&sectionHeaderTable[1],                            /* .text header */
		                   stringIndexFromSectionIndex(stringTable.bytes, 1), /* String table index */
		                   SHT_PROGBITS,                                      /* Type of segment*/
		                   SHF_ALLOC + SHF_EXECINSTR,                         /* Section permissions */
		                   TEXT_MEM_LOC,                                      /* Section memory location */
		                   TEXT_FILE_LOC,                                     /* Section memory location */
		                   text.size                                          /* Segment size */
		                   );
		setupSectionHeader(&sectionHeaderTable[2], /* .data header */
		                   stringIndexFromSectionIndex(stringTable.bytes, 2),
		                   SHT_PROGBITS,
		                   SHF_ALLOC + SHF_WRITE,
		                   TEXT_MEM_LOC + text.size,
		                   TEXT_FILE_LOC + text.size,
		                   data.size);
		setupSectionHeader(&sectionHeaderTable[3], /* .shrtrab header */
		                   stringIndexFromSectionIndex(stringTable.bytes, 3),
		                   SHT_STRTAB,
		                   0,
		                   0,
		                   TEXT_FILE_LOC + text.size + data.size,
		                   stringTable.size);

		ELFHeader.e_shstrndx = 3; // Set which section header describes the string table

		ELFHeader.e_shoff = SEC_HEADER_NUM == 0 ? 0x0 : TEXT_FILE_LOC + text.size + data.size + stringTable.size; // Finally determine the section header offset

		debugPrintf("Writing ELF header\n");
		fwrite(&ELFHeader, 1, sizeof(ELFHeader), writeFile); // Write header information

		debugPrintf("Writing program header table\n");
		fwrite(&programHeaderTable, 1, sizeof(programHeaderTable), writeFile); // Write program header table

		debugPrintf("Writing .text section of size %i bytes\n", text.size);
		fwrite(text.bytes, 1, text.size, writeFile); // Write .text
		debugPrintf("Writing .data section of size %i bytes\n", data.size);
		fwrite(data.bytes, 1, data.size, writeFile); // Write .data
		debugPrintf("Writing .shrtrab section of size %i bytes\n", stringTable.size);
		fwrite(stringTable.bytes, 1, stringTable.size, writeFile); // Write .shrtrab

		debugPrintf("Writing section header, %i sections\n", ELFHeader.e_shnum);
		fwrite(&sectionHeaderTable, 1, sizeof(sectionHeaderTable), writeFile); // Write section header table

		debugPrintf("Entry point: %#08x\n", ENTRY_POINT);

		debugPrintf("Done processing file %s\n", arguments.args[i]);
	}
	fclose(readFile);  // Close read file pointer
	fclose(writeFile); // Close write file pointer
}
