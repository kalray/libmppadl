/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
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

int mppa_dl_autotraces;

#define MPPA_DL_LOG(level, fmt, ...)			\
	if (mppa_dl_loglevel >= level)			\
		fprintf (stderr, fmt, ##__VA_ARGS__);

/**
 * Set the verbosity level of the library. Four log levels are supported.
 *   - level 0: logs are disabled.
 *   - level 1: enable minimal log level, i.e., control flow.
 *   - level 2: level 1 + more details about relocations.
 *   - level 3: level 1 + level 2 + autotraces logs.
 *
 * The default log level is 0.
 */
void mppa_dl_set_loglevel(int level);

void mppa_dl_errno(int errno);
int mppa_dl_errno_get_status();

extern char *mppa_dl_error(void);

#endif
