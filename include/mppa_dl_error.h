/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#ifndef MPPA_DL_ERROR_H
#define MPPA_DL_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <gelf.h>

int __mppa_dl_loglevel;

enum MPPA_DL_ERRNO {
	E_NONE,
	E_ELF_OLD,
	E_ELF_MEM,
	E_ELF_PHDRNUM,
	E_ELF_PHDR,
	E_ELF_SHDR,
	E_ELF_END,
	E_ELF_SYM,
	E_MEM_ALIGN,
	E_UNKN_RELOC
};

int mppa_dl_errno_status;

void mppa_dl_errno(int errno);

extern char *mppa_dl_error(void);

#endif
