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
#include <gelf.h>

enum MPPA_DL_ERRNO {
	E_NONE,
	E_ELF_OLD,
	E_ELF_MEM,
	E_ELF_PHDRNUM,
	E_ELF_PHDR
};

int mppa_dl_errno_status;

void mppa_dl_errno(int errno);

extern char *mppa_dl_error(void);

#endif
