/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#include "mppa_dl_mem.h"

#ifdef __rtems__
void *(*mppa_dl_memalign)(size_t, size_t) = mppa_dl_rtems_memalign_wrapper;
#else
void *(*mppa_dl_memalign)(size_t, size_t) = memalign;
#endif
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

#ifdef __rtems__
void * mppa_dl_rtems_memalign_wrapper(size_t boundary, size_t size) {
	void *b;
	int r = posix_memalign(&b, boundary, size);
	if (r != 0) {
		return NULL;
	} else {
		return b;
	}
}
#endif
