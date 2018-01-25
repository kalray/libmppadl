/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_mem.h"

void *(*mppa_dl_memalign)(size_t, size_t) = memalign;
void *(*mppa_dl_malloc)(size_t) = malloc;
void (*mppa_dl_free)(void *) = free;

void mppa_dl_set_memalign(void *(*ptr)(size_t, size_t))
{
	if (mppa_dl_loglevel > 0)
		fprintf(stderr, "> mppa_dl_set_memalign()\n");

	mppa_dl_memalign = ptr;

	if (mppa_dl_loglevel > 0)
		fprintf(stderr, "< mppa_dl_set_memalign()\n");
}

void mppa_dl_set_malloc(void *(*ptr)(size_t))
{
	if (mppa_dl_loglevel > 0)
		fprintf(stderr, "> mppa_dl_set_malloc()\n");

	mppa_dl_malloc = ptr;

	if (mppa_dl_loglevel > 0)
		fprintf(stderr, "< mppa_dl_set_malloc()\n");
}

void mppa_dl_set_free(void (*ptr)(void *))
{
	if (mppa_dl_loglevel > 0)
		fprintf(stderr, "> mppa_dl_set_free()\n");

	mppa_dl_free = ptr;

	if (mppa_dl_loglevel > 0)
		fprintf(stderr, "< mppa_dl_set_free()\n");
}
