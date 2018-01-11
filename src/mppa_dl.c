/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#include "mppa_dl.h"

void *mppa_dl_load(const char *image, size_t size)
{
	mppa_dl_errno(E_NONE);
	mppa_dl_handle_t *hdl = NULL;

	/* querying the current operating version of the ELF library */
	if (elf_version(EV_CURRENT) == EV_NONE) {
		mppa_dl_errno(E_ELF_OLD);
		return (void*)hdl;
	}

	hdl = (mppa_dl_handle_t*)malloc(sizeof(mppa_dl_handle_t));

	/* process the ELF image present in memory */
	hdl->elf_desc = elf_memory((char*)image, size);
	if (hdl->elf_desc == NULL) {
		mppa_dl_errno(E_ELF_MEM);
		return (void*)hdl;
	}

	/* allocate memory to load needed ELF segments */
	mppa_dl_load_segments_memsz(hdl);

	return (void*)hdl;
}
