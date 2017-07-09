# **Y**et **A**nother **B**rain**F**uck **C**ompiler

## Description
YABFC is an optimizing compiler for the brainfuck language that uses no dependencies apart from standard POSIX headers.
The output format is a 64 bit ELF executable file.  Be sure to visit the [wiki!](https://github.com/cameronswinoga/yabfc/wiki/Generating-executable-files-from-scratch)

## Building
``` BASH
make && sudo make install
```

## Usage
``` Text
Usage: yabfc [OPTION...] Files...
Compiles one or multiple brainfuck files to the ELF file format, in 64 bit mode

  -o, --output=FILE          Output to FILE
  -O, --optimize[=LEVEL]     Set optimization to LEVEL 1,2 or 3 (default 3)
  -q, -s, --quiet, --silent  Do not produce any output
  -v, --verbose[=LEVEL]      Set verbosity to LEVEL 1,2 or 3 (default 2)
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <cameronswinoga@gmail.com>.
```

## Testing
Make sure to have `valgrind` installed, then run `./integration_tests.sh`.  Valgrind is necessary to have a sandbox environment where 64 bit ELF's can run on any host OS.

## Implementation
The data pointer in the executable is representated by the assembly stack pointer, as such and new stack allocation is guaranteed to be zeroed initially.  Because of this, as well as the executable being set in 64 bit mode, allows YABFC to have an extremely large value range.  The size for the data pointer is the stack size and the cell size is 2<sup>64</sup>.
