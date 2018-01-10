/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_error.h"

void mppa_dl_failure(char *file, int line)
{
	fprintf(stderr, "libmppadl failed in file %s at line # %d", file, line);
	exit(EXIT_FAILURE);
}
