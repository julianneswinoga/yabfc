#ifndef ELFHELPER_H_
#define ELFHELPER_H_

#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ORG 0x4000000 // Origin memory address

#define PGM_HEADER_TBL_LOC sizeof(Elf64_Ehdr) // Program header location
#define PGM_HEADER_SIZE sizeof(Elf64_Phdr)    // Program header size
#define SEC_HEADER_SIZE sizeof(Elf64_Shdr)    // Section header size
#define PGM_HEADER_NUM 2                      // Number of program headers
#define SEC_HEADER_NUM 4                      // Number of section headers

#define TEXT_FILE_LOC (PGM_HEADER_TBL_LOC + (PGM_HEADER_NUM * PGM_HEADER_SIZE)) // .text file location
#define TEXT_MEM_LOC (ORG + TEXT_FILE_LOC)                                      // .text memory location

#define ENTRY_POINT TEXT_MEM_LOC // Executable entry point

// Function declarations
void setupELFHeader(Elf64_Ehdr *, uint64_t, uint64_t, uint64_t, uint16_t, uint16_t, uint16_t, uint16_t);
void setupSectionHeader(Elf64_Shdr *, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
void setupprogramHeader(Elf64_Phdr *, uint64_t, uint64_t, uint64_t, uint64_t);

// Struct to define a section
typedef struct {
	uint32_t size;
	uint8_t *bytes;
} SECTION;

#endif // ELFHELPER_H_
