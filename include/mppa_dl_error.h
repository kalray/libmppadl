/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#ifndef MPPA_DL_ERROR_H
#define MPPA_DL_ERROR_H

#define VERBOSE 0

#include <stdio.h>
#include <stdlib.h>
#include <gelf.h>

enum MPPA_DL_ERRNO {
	E_NONE,
	E_MEM_ALIGN,
	E_RELOC,
	E_PLT_RELOC,
	E_END_CHAIN,
	E_SYM_OUT,
	E_NO_SYM,
	E_INIT_HDL
};

int mppa_dl_errno_status;

void mppa_dl_errno(int errno);

extern char *mppa_dl_error(void);

#endif
