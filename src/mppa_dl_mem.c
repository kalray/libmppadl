/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_mem.h"

mppa_dl_memalign_t mppa_dl_memalign = memalign;
mppa_dl_malloc_t mppa_dl_malloc = malloc;
mppa_dl_free_t mppa_dl_free = free;

void mppa_dl_set_memalign(void *(*ptr)(size_t, size_t))
{
	MPPA_DL_LOG(1, "> mppa_dl_set_memalign()\n");

	mppa_dl_memalign = ptr;

	MPPA_DL_LOG(1, "< mppa_dl_set_memalign()\n");
}

void mppa_dl_set_malloc(void *(*ptr)(size_t))
{
	MPPA_DL_LOG(1, "> mppa_dl_set_malloc()\n");

	mppa_dl_malloc = ptr;

	MPPA_DL_LOG(1, "< mppa_dl_set_malloc()\n");
}

void mppa_dl_set_free(void (*ptr)(void *))
{
	MPPA_DL_LOG(1, "> mppa_dl_set_free()\n");

	mppa_dl_free = ptr;

	MPPA_DL_LOG(1, "< mppa_dl_set_free()\n");
}
