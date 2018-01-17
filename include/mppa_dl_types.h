/**
 * Copyright (C) 2018 Kalray SA.
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
	Elf     *hdl_elf;     /* ELF descriptor */
	size_t   hdl_phdrnum; /* Number of segments in ELF memory image */
	void    *hdl_addr;    /* Starting address of the loaded ELF image */
	size_t   hdl_strtab;  /* strtab offset */
	/*size_t hashoff;*/
	Elf_Scn *hdl_dynsym;  /* Dynamic linking symbols section */

	mppa_dl_shdr_t *hdl_reloc_l; /* List of relocation sections */
} mppa_dl_handle_t;

#endif
