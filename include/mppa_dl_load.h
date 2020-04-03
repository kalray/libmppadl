/**
 * Copyright (C) 2018 Kalray SA.
 */

#ifndef MPPA_DL_LOAD_H
#define MPPA_DL_LOAD_H

#include <string.h>
#include <gelf.h>

#include "mppa_dl_types.h"
#include "mppa_dl_sym.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl);

int mppa_dl_apply_rela(mppa_dl_handle_t *hdl, ElfKVX_Rela rel);

enum MPPA_DL_ERRNO mppa_dl_init_handle(mppa_dl_handle_t *h, ElfKVX_Dyn *dyn, 
			void *off, mppa_dl_handle_t *parent, int availability);

#endif
