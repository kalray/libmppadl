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
	E_INIT_HDL,
	E_WRONG_FLAG,
	E_UNSUP_TLS
};

#define MPPA_DL_LOG(level, fmt, ...)			\
  if (mppa_dl_get_loglevel() >= level)			\
    fprintf (stderr, fmt, ##__VA_ARGS__);

void mppa_dl_set_loglevel(int level);
int mppa_dl_get_loglevel(void);

void mppa_dl_errno(int errno);
int mppa_dl_errno_get_status(void);

char *mppa_dl_error(void);

#endif
