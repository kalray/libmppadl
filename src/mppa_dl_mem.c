/**
 * Copyright (C) 2018 Kalray SA.
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

#ifdef __rtems__
void * mppa_dl_rtems_memalign_wrapper(size_t boundary, size_t size)
{
	MPPA_DL_LOG(1, "> mppa_dl_rtems_memalign_wrapper()\n");

	void *b;
	int r = posix_memalign(&b, boundary, size);

	MPPA_DL_LOG(1, "< mppa_dl_rtems_memalign_wrapper()\n");

	if (r != 0) {
		return NULL;
	} else {
		return b;
	}
}
#endif
