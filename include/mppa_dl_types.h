/**
 * Copyright (C) 2018 Kalray SA.
 */

#ifndef MPPA_DL_TYPES_H
#define MPPA_DL_TYPES_H

typedef struct {
	Elf *elf_desc;
	void *mem_addr;
} mppa_dl_handle_t;

#endif
