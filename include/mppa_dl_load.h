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
#include "mppa_dl_error.h"

/**
 * Determine memory size required to load segments into memory.
 * Compute the total memory size required by loadable segments (PT_LOAD), in
 * bytes, which is the last PT_LOAD segment plus its size in memory. PT_LOAD
 * segments are ordered in ascending order, so taking the last one is safe,
 * return 0 on error (to be checked with mppa_dl_error().
 */
size_t mppa_dl_load_segments_memsz(mppa_dl_handle_t *hdl);

/**
 * Search the maximum alignment among loadable segments.
 * Find the maximum alignment required by loadable segments (PT_LOAD), in bytes,
 * which is the maximum alignment of all the PT_LOAD segments. Return 0 on error
 * (to be checked with mppa_dl_error().
 */
size_t mppa_dl_load_segments_align(mppa_dl_handle_t *hdl);

int mppa_dl_add_relascn(mppa_dl_handle_t *hdl, Elf_Scn *scn);
int mppa_dl_apply_reloc(mppa_dl_handle_t *hdl);

#endif
