#ifndef ELFHELPER_H_
#define ELFHELPER_H_

#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ORG 0x4000000

#define PGM_HEADER_TBL_LOC sizeof(Elf64_Ehdr)
#define PGM_HEADER_SIZE sizeof(Elf64_Phdr)
#define SEC_HEADER_SIZE sizeof(Elf64_Shdr)
#define PGM_HEADER_NUM 2
#define SEC_HEADER_NUM 4

#define TEXT_FILE_LOC (PGM_HEADER_TBL_LOC + (PGM_HEADER_NUM * PGM_HEADER_SIZE))
#define TEXT_MEM_LOC (ORG + TEXT_FILE_LOC /*0x78*/)

#define ENTRY_POINT TEXT_MEM_LOC

typedef struct {
	uint32_t size;
	uint8_t *bytes;
} SECTION;

void setupELFHeader(Elf64_Ehdr *, uint64_t, uint64_t, uint64_t, uint16_t, uint16_t, uint16_t, uint16_t);
void setupSectionHeader(Elf64_Shdr *, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
void setupprogramHeader(Elf64_Phdr *, uint64_t, uint64_t, uint64_t, uint64_t);

#endif // ELFHELPER_H_
