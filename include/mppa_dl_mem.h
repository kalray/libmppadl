/**
 * Copyright (C) 2018 Kalray SA.
 */

#ifndef MPPA_DL_MEM_H
#define MPPA_DL_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "mppa_dl_error.h"

typedef void *(*mppa_dl_memalign_t)(size_t, size_t);
typedef void *(*mppa_dl_malloc_t)(size_t);
typedef void (*mppa_dl_free_t)(void *);

extern mppa_dl_memalign_t mppa_dl_memalign;
extern mppa_dl_malloc_t mppa_dl_malloc;
extern mppa_dl_free_t mppa_dl_free;

void mppa_dl_set_memalign(mppa_dl_memalign_t);
void mppa_dl_set_malloc(mppa_dl_malloc_t);
void mppa_dl_set_free(mppa_dl_free_t);

#ifdef __rtems__
void * mppa_dl_rtems_memalign_wrapper(size_t boundary, size_t size);
#endif

#endif
