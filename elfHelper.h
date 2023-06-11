#ifndef ELFHELPER_H_
#define ELFHELPER_H_

#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ORG (0x4000000)                                                          // Origin memory address

#define PGM_HEADER_TBL_LOC (sizeof(Elf64_Ehdr))                                  // Program header location
#define PGM_HEADER_SIZE (sizeof(Elf64_Phdr))                                     // Program header size
#define SEC_HEADER_SIZE (sizeof(Elf64_Shdr))                                     // Section header size
#define PGM_HEADER_NUM (2)                                                       // Number of program headers
#define SEC_HEADER_NUM (4)                                                       // Number of section headers

#define TEXT_FILE_LOC (PGM_HEADER_TBL_LOC + (PGM_HEADER_NUM * PGM_HEADER_SIZE))  // .text file location
#define TEXT_MEM_LOC (ORG + TEXT_FILE_LOC)                                       // .text memory location

#define ENTRY_POINT TEXT_MEM_LOC                                                 // Executable entry point

// Function declarations
void setupELFHeader(Elf64_Ehdr *ELFHeader,
                    uint64_t entry,
                    uint64_t phoff,
                    uint64_t shoff,
                    uint16_t phentsize,
                    uint16_t phnum,
                    uint16_t shentsize,
                    uint16_t shnum);
void setupProgramHeader(Elf64_Phdr *programHeader, uint64_t flags, uint64_t poffset, uint64_t moffset, uint64_t size);
void setupSectionHeader(Elf64_Shdr *sectionHeader,
                        uint64_t sh_name,
                        uint64_t sh_type,
                        uint64_t sh_flags,
                        uint64_t sh_addr,
                        uint64_t sh_offset,
                        uint64_t sh_size);

// Struct to define a section
typedef struct {
    uint32_t size;
    uint8_t *bytes;
} SECTION;

#endif  // ELFHELPER_H_
