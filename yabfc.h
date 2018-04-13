#ifndef YABFC_H_
#define YABFC_H_

#include "arguments.h"
#include "assembly.h"
#include "helpers.h"
#include "optimize.h"
#include <argp.h>
#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define VALID_COMMANDS ("+-<>[],.")

// Function declarations
static error_t parse_opt(int, char *, struct argp_state *);

// Author information
const char *argp_program_version     = "yabfc 1.0";
const char *argp_program_bug_address = "<cameronswinoga@gmail.com>";

static char doc[] =
    "Compiles one or multiple brainfuck files to "
    "the ELF file format, in 64 bit mode";

static char args_doc[] = "Files...";

//The options that can be applied to the program
static struct argp_option options[] = {
    {"quiet", 'q', 0, 0, "Do not produce any output"},
    {"silent", 's', 0, OPTION_ALIAS},
    {"output", 'o', "FILE", 0, "Output to FILE"},
    {"verbose", 'v', "LEVEL", OPTION_ARG_OPTIONAL, "Set verbosity to LEVEL 1,2 or 3 (default 2)"},
    {"optimize", 'O', "LEVEL", OPTION_ARG_OPTIONAL, "Set optimization to LEVEL 1,2 or 3 (default 3)"},

    {0}};

// Structure to hold the arguments
struct argumentStruct {
	char **inputFiles;
	int    silent;
	int    verbose;
	int    optimize;
	char * output_file;
};

struct argumentStruct arguments = {
    .silent      = 0,
    .verbose     = 1,
    .optimize    = 3,
    .output_file = ""}; // Initialize an argument struct

globalOptionsStruct globalOptions = {
    .verbose    = 0,
    .optimize   = 0,
    .silent     = false,
    .outputFile = ""}; // Program arguments that should be globally shared

static struct argp argp = {options, parse_opt, args_doc, doc};

#endif // YABFC_H_
