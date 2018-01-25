/**
 * Copyright (C) 2018 Kalray SA.
 *
 *
 * \file mppa_dl_error.h
 * \brief Failure management.
 */

#ifndef MPPA_DL_ERROR_H
#define MPPA_DL_ERROR_H

#include <stdio.h>
#include <stdlib.h>

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

int mppa_dl_loglevel;
int mppa_dl_errno_status;

void mppa_dl_set_loglevel(int level);

void mppa_dl_errno(int errno);
int mppa_dl_errno_get_status();

extern char *mppa_dl_error(void);

#endif
