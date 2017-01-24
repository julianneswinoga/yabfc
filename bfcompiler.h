#ifndef BFCOMPILER_H_
#define BFCOMPILER__H_

#include <argp.h>
#include <elf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static error_t parse_opt(int, char *, struct argp_state *);

const char *argp_program_version     = "bfcompiler 0.1";
const char *argp_program_bug_address = "<cameronswinoga@gmail.com>";
static char doc[]		     = "Documentation";
static char args_doc[]		     = "ARG1 ARG2";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose", 'v', 0, 0, "Produce verbose output"},
    {"quiet", 'q', 0, 0, "Don't produce any output"},
    {"silent", 's', 0, OPTION_ALIAS},
    {"output", 'o', "FILE", 0, "Output to FILE instead of standard output"},
    {0}};

struct arguments {
  char *args[2]; /* arg1 & arg2 */
  int   silent, verbose;
  char *output_file;
};

static struct argp argp = {options, parse_opt, args_doc, doc};

#endif // BFCOMPILER_H_
