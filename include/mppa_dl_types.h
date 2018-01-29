/**
 * Copyright (C) 2018 Kalray SA.
 */

#ifndef MPPA_DL_TYPES_H
#define MPPA_DL_TYPES_H

#define MAX(a,b) (((a)>(b))?(a):(b))

#ifdef MPPA_DL_K1_64
typedef Elf64_Dyn  ElfK1_Dyn;
typedef Elf64_Word ElfK1_Word;
typedef Elf64_Rela ElfK1_Rela;
typedef Elf64_Sym  ElfK1_Sym;
typedef Elf64_Addr ElfK1_Addr;
typedef Elf64_Half ElfK1_Half;
typedef Elf64_Ehdr ElfK1_Ehdr;
typedef Elf64_Shdr ElfK1_Shdr;
typedef Elf64_Phdr ElfK1_Phdr;

#define ELFK1_R_TYPE(i) ELF64_R_TYPE((i))
#define ELFK1_R_SYM(i)  ELF64_R_SYM((i))
#else
typedef Elf32_Dyn  ElfK1_Dyn;
typedef Elf32_Word ElfK1_Word;
typedef Elf32_Rela ElfK1_Rela;
typedef Elf32_Sym  ElfK1_Sym;
typedef Elf32_Addr ElfK1_Addr;
typedef Elf32_Half ElfK1_Half;
typedef Elf32_Ehdr ElfK1_Ehdr;
typedef Elf32_Shdr ElfK1_Shdr;
typedef Elf32_Phdr ElfK1_Phdr;

#define ELFK1_R_TYPE(i) ELF32_R_TYPE((i))
#define ELFK1_R_SYM(i)  ELF32_R_SYM((i))
#endif

extern __attribute__((weak)) ElfK1_Dyn _DYNAMIC[];

enum MPPA_DL_RELOCATIONS {
	R_K1_32 = 2,
	R_K1_GLOB_DAT = 17,
	R_K1_JMP_SLOT = 31,
	R_K1_RELATIVE = 32
};

typedef struct mppa_dl_handle {
	ElfK1_Addr *addr;
	ElfK1_Half type;

	ElfK1_Word nbucket;
	ElfK1_Word nchain;
	ElfK1_Word *bucket;
	ElfK1_Word *chain;

	ElfK1_Word *hash;

	ElfK1_Rela *rela;
	size_t     relan;

	void       *jmprel;
	ElfK1_Word pltrel;
	size_t     pltreln;

	char       *strtab;
	size_t     strsz;
	ElfK1_Sym  *symtab;

	struct mppa_dl_handle *parent;
	struct mppa_dl_handle *child;
} mppa_dl_handle_t;

#endif
