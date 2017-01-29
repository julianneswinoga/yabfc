#ifndef YABFC_H_
#define YABFC_H_

#include "arguments.h"
#include "helpers.h"
#include <argp.h>
#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

globalOptionsStruct globalOptions = {
    .numFiles = 0, .verbose = false, .silent = false, .outputFile = ""};

static error_t parse_opt(int, char *, struct argp_state *);

const char *argp_program_version     = "bfcompiler 0.1";
const char *argp_program_bug_address = "<cameronswinoga@gmail.com>";
static char doc[]                    = "Documentation";
static char args_doc[]               = "InputFiles";

/* The options we understand. */
static struct argp_option options[] = {{"verbose", 'v', 0, 0, "Produce verbose output"},
                                       {"quiet", 'q', 0, 0, "Don't produce any output"},
                                       {"silent", 's', 0, OPTION_ALIAS},
                                       {"output", 'o', "FILE", 0, "Output to FILE"},
                                       {0}};

struct argumentStruct {
	char *args[64];
	int   silent, verbose;
	char *output_file;
	int   numFiles;
};

struct argumentStruct arguments = {.silent = 0, .verbose = 0, .output_file = "", .numFiles = 0};

static struct argp argp = {options, parse_opt, args_doc, doc};

#endif // YABFC_H_
