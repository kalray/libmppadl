/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#ifndef MPPA_DL_SYM_H
#define MPPA_DL_SYM_H

#include <string.h>
#include <gelf.h>
#include <stdio.h>

#include "mppa_dl_types.h"
#include "mppa_dl_error.h"

void *mppa_dl_sym_lookup(mppa_dl_handle_t *hdl, const char *symbol);

void *mppa_dl_sym_lookup2(mppa_dl_handle_t *hdl, const char *symbol);

#endif
