/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#ifndef MPPA_DL_TYPES_H
#define MPPA_DL_TYPES_H

enum MPPA_DL_RELOCATIONS {
	R_K1_RELATIVE = 32
};

typedef struct mppa_dl_shdr {
	Elf_Scn *scn;
	struct mppa_dl_shdr *next;
} mppa_dl_shdr_t;

typedef struct {
	Elf *elf_desc; /* ELF descriptor */
	size_t phdrnum; /* Number of segments in ELF memory image */
	void *mem_addr; /* Starting address of the loaded ELF image */

	size_t strtaboff;

	mppa_dl_shdr_t *s_reloc; /* List of relocation sections */
	Elf_Scn *s_symtab; /* Symbol table section */
	Elf_Scn *s_strtab;
	Elf_Scn *s_dynsym; /* Dynamic linking symbols section */

} mppa_dl_handle_t;

#endif
