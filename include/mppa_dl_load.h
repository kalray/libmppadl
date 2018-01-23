/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#ifndef MPPA_DL_LOAD_H
#define MPPA_DL_LOAD_H

#include <string.h>
#include <gelf.h>

#include "mppa_dl_types.h"
#include "mppa_dl_sym.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl);

int mppa_dl_apply_rela(mppa_dl_handle_t *hdl, ElfK1_Rela rel);

int mppa_dl_init_handle(mppa_dl_handle_t *h, ElfK1_Dyn *dyn, 
			void *off, mppa_dl_handle_t *parent);

#endif
