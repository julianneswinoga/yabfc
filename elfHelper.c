#include "elfHelper.h"

/**
 * Helper method to fill in an ELF Header structure
 * @param ELFHeader The 64 bit ELF Header structure to modify
 * @param entry     Program memory entry location
 * @param phoff     Program header table file offset
 * @param shoff     Section header table file offset
 * @param phentsize Program header size
 * @param phnum     Number of program headers
 * @param shentsize Section header size
 * @param shnum     Number of section headers
 */
void setupELFHeader(Elf64_Ehdr *ELFHeader,
                    uint64_t    entry,
                    uint64_t    phoff,
                    uint64_t    shoff,
                    uint16_t    phentsize,
                    uint16_t    phnum,
                    uint16_t    shentsize,
                    uint16_t    shnum) {
	ELFHeader->e_ident[EI_MAG0]       = 0x7f;
	ELFHeader->e_ident[EI_MAG1]       = 'E';
	ELFHeader->e_ident[EI_MAG2]       = 'L';
	ELFHeader->e_ident[EI_MAG3]       = 'F';
	ELFHeader->e_ident[EI_CLASS]      = ELFCLASS64;    // 64 bit ELF
	ELFHeader->e_ident[EI_DATA]       = ELFDATA2LSB;   // little-endian
	ELFHeader->e_ident[EI_VERSION]    = EV_CURRENT;    // Current version
	ELFHeader->e_ident[EI_OSABI]      = ELFOSABI_SYSV; // UNIX System V ABI
	ELFHeader->e_ident[EI_ABIVERSION] = 0x0;           // ABI version needs to be 0

	for (int i = EI_PAD; i < EI_NIDENT; i++) ELFHeader->e_ident[i] = 0x0; // Zero padding

	ELFHeader->e_type    = ET_EXEC;            // Executable file
	ELFHeader->e_machine = EM_X86_64;          // AMD x86-64
	ELFHeader->e_version = EV_CURRENT;         // Current version
	ELFHeader->e_entry   = entry;              // Entry point of program (>128 MB)
	ELFHeader->e_phoff   = phoff;              // Program header table offset
	ELFHeader->e_shoff   = shoff;              // Section header table offset
	ELFHeader->e_flags   = 0x0;                // Processor specific flags
	ELFHeader->e_ehsize  = sizeof(Elf64_Ehdr); // ELF Header size

	ELFHeader->e_phentsize = phentsize; // Size of each program header
	if (phnum > PN_XNUM) {              // Number of entries in program header table
		ELFHeader->e_phnum = PN_XNUM;
		printf("WARNING: Elf64_Ehdr.e_phnum is greater than PN_XNUM, "
		       "sh_info should be set to the number of entries in the program header table\n");
	} else {
		ELFHeader->e_phnum = phnum;
	}

	ELFHeader->e_shentsize = shentsize; // Section header size, in bytes
	if (shnum > SHN_LORESERVE) {        // Number of entries in section header
		ELFHeader->e_shnum = 0x0;
		printf("WARNING: Elf64_Ehdr.e_shnum is greater than SHN_LORESERVE, "
		       "sh_size should be set to the number of entries in the program header table\n");
	} else {
		ELFHeader->e_shnum = shnum;
	}

	int e_shstrndx = SHN_UNDEF; // Section header table index of the entry associated with thes ection name string table, to be set later

	if (e_shstrndx > SHN_LORESERVE) {
		ELFHeader->e_shstrndx = SHN_XINDEX;
		printf("WARNING: Elf64_Ehdr.e_shstrndx is greater than SHN_LORESERVE, "
		       "sh_link should be set to the number of entries in the program header table\n");
	} else {
		ELFHeader->e_shstrndx = e_shstrndx;
	}
}

/**
 * Helper method to fill in ELF section header
 * @param sectionHeader Section header to modify
 * @param sh_name       Section name
 * @param sh_type       Section type
 * @param sh_flags      Section flags
 * @param sh_addr       Section memory address
 * @param sh_offset     Section file address
 * @param sh_size       Section size
 */
void setupSectionHeader(Elf64_Shdr *sectionHeader, uint64_t sh_name, uint64_t sh_type, uint64_t sh_flags, uint64_t sh_addr, uint64_t sh_offset, uint64_t sh_size) {
	sectionHeader->sh_name      = sh_name;
	sectionHeader->sh_type      = sh_type;
	sectionHeader->sh_flags     = sh_flags;
	sectionHeader->sh_addr      = sh_addr;
	sectionHeader->sh_offset    = sh_offset;
	sectionHeader->sh_size      = sh_size;
	sectionHeader->sh_link      = 0; // Currently unused
	sectionHeader->sh_info      = 0;
	sectionHeader->sh_addralign = 0;
	sectionHeader->sh_entsize   = 0;
}

void setupprogramHeader(Elf64_Phdr *programHeaderTable, uint64_t flags, uint64_t poffset, uint64_t moffset, uint64_t size) {
	programHeaderTable->p_type   = PT_LOAD;
	programHeaderTable->p_flags  = flags;
	programHeaderTable->p_offset = poffset;
	programHeaderTable->p_vaddr  = moffset;
	programHeaderTable->p_paddr  = moffset;
	programHeaderTable->p_filesz = size;
	programHeaderTable->p_memsz  = size;

	programHeaderTable->p_align = 0x0;
}
