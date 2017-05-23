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
		case 'v': arguments->verbose     = arg ? atoi(arg) : 2; break;
		case 'o': arguments->output_file = arg; break;
		case 'O': arguments->optimize    = arg ? atoi(arg) : 2; break;

		case ARGP_KEY_NO_ARGS:
			argp_usage(state);

		case ARGP_KEY_ARGS:
			arguments->inputFiles = &state->argv[state->next];
			state->next           = state->argc;
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
 * Main function
 * @param  argc Argument count
 * @param  argv arguments
 * @return      Program return value
 */
int main(int argc, char *argv[]) {
	int   numFiles;
	char *outputFilename;
	FILE *readFile, *writeFile; // Read and write file pointers

	argp_parse(&argp, argc, argv, 0, 0, &arguments); // Parse the command line arguments

	for (numFiles = 0; arguments.inputFiles[numFiles]; numFiles++) // Count the number of input files
		;
	globalOptions.optimize   = arguments.optimize;
	globalOptions.verbose    = arguments.verbose;
	globalOptions.silent     = arguments.silent;
	globalOptions.outputFile = arguments.output_file;

	debugPrintf(1, "Optimization level: %i\nVerbosity level: %i\nSilent mode: %s\nOutput file: %s\n",
	            globalOptions.optimize,
	            globalOptions.verbose,
	            globalOptions.silent ? "true" : "false",
	            globalOptions.outputFile);

	if (numFiles > 1) {
		debugPrintf(1, "WARNING: Multiple files specified, ignoring output file flag\n");
		globalOptions.outputFile = "";
	}

	for (int i = 0; arguments.inputFiles[i]; i++) { // Loop through the input files
		debugPrintf(1, "Opening file %s\n", arguments.inputFiles[i]);
		readFile = fopen(arguments.inputFiles[i], "r"); // Open file for reading
		if (readFile == NULL) {
			perror("Error opening file: ");
			exit(1);
		}
		if (strcmp(globalOptions.outputFile, "") != 0) {
			writeFile = fopen(globalOptions.outputFile, "w+");
		} else {
			outputFilename = filenameWithoutExtension(arguments.inputFiles[i]);
			debugPrintf(1, "Output file: %s\n", outputFilename);
			writeFile = fopen(outputFilename, "w+");
		}

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

		debugPrintf(2, "Constructing .text section\n");

		INSTRUCTIONS instructions = {
		    .size        = 0,
		    .instruction = malloc(0)};

		debugPrintf(2, "Program as read from file: ");
		char readCharacter = 0;
		while ((readCharacter = fgetc(readFile)) != EOF) {
			if (strpbrk(VALID_COMMANDS, &readCharacter) != NULL) {
				debugPrintf(2, "%c", readCharacter);
				INSTRUCTION tempInstruction = {
				    .type         = readCharacter,
				    .bracketMatch = -1};

				instructions.instruction                      = (INSTRUCTION *)realloc(instructions.instruction, (instructions.size + 1) * sizeof(INSTRUCTION));
				instructions.instruction[instructions.size++] = tempInstruction;
			}
		}
		debugPrintf(2, "\n");

		debugPrintf(2, "Starting machine code generation\n");
		CODE code = {
		    .size  = 0,
		    .bytes = malloc(0)};

		construct_START(&code);

		int relativeBracket, lookahead;
		for (int i = 0; i < instructions.size; i++) {

			if (optimize_compress_PTR(&instructions, &i, &code) ||
			    optimize_compress_ADDSUB(&instructions, &i, &code)) {
				continue;
			}

			switch (instructions.instruction[i].type) {
				case '+':
					construct_ADD(&code, 1);
					break;
				case '-':
					construct_SUB(&code, 1);
					break;
				case '<':
					construct_ADDESP(&code, 4);
					break;
				case '>':
					construct_SUBESP(&code, 4);
					break;
				case '[':
					if ((relativeBracket = get_matching_bracket(&instructions, i)) == -1) {
						fprintf(stderr, "Opening bracket does not have a matching closing bracket at position %i!\n", i);
						exit(1);
					}
					instructions.instruction[i].bracketMatch                   = relativeBracket;
					instructions.instruction[i + relativeBracket].bracketMatch = relativeBracket;
					debugPrintf(3, "Opening bracket @ %i jumps forward by %i\n", i, relativeBracket);
					construct_LPSTART(&code);
					break;
				case ']':
					if (instructions.instruction[i].bracketMatch == -1) {
						fprintf(stderr, "Closing bracket does not have a matching opening bracket at position %i!\n", i);
						exit(1);
					}
					debugPrintf(3, "Closing bracket @ %i jumps backward by %i\n", i, instructions.instruction[i].bracketMatch);
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

		debugPrintf(2, "Code bytesize: %i\nSection bytesize: %i\n", code.size, text.size);

		debugPrintf(2, "Constructing .data section\n");
		uint8_t tempData = '0';
		for (int i = 0; i < 0; i++)
			addSectionData(&data, (uint8_t *)&tempData, sizeof(tempData)); // Add some example data

		debugPrintf(2, "Constructing .shrtrab section\n");
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

		debugPrintf(2, "Writing ELF header\n");
		fwrite(&ELFHeader, 1, sizeof(ELFHeader), writeFile); // Write header information

		debugPrintf(2, "Writing program header table\n");
		fwrite(&programHeaderTable, 1, sizeof(programHeaderTable), writeFile); // Write program header table

		debugPrintf(2, "Writing .text section of size %i bytes\n", text.size);
		fwrite(text.bytes, 1, text.size, writeFile); // Write .text
		debugPrintf(2, "Writing .data section of size %i bytes\n", data.size);
		fwrite(data.bytes, 1, data.size, writeFile); // Write .data
		debugPrintf(2, "Writing .shrtrab section of size %i bytes\n", stringTable.size);
		fwrite(stringTable.bytes, 1, stringTable.size, writeFile); // Write .shrtrab

		debugPrintf(2, "Writing section header, %i sections\n", ELFHeader.e_shnum);
		fwrite(&sectionHeaderTable, 1, sizeof(sectionHeaderTable), writeFile); // Write section header table

		debugPrintf(1, "Entry point: %#08x\n", ENTRY_POINT);

		debugPrintf(1, "Done processing file %s\n", arguments.inputFiles[i]);
	}
	fclose(readFile);  // Close read file pointer
	fclose(writeFile); // Close write file pointer
}
