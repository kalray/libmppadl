/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#include "mppa_dl.h"

void *mppa_dl_sym(void *handle, const char* symbol)
{
	if (handle == NULL && symbol == NULL)
		return NULL;
	return NULL;
}
