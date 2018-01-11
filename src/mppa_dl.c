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
	GElf_Phdr phdr;
	int i;

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

	/* retrieve the number of EFL program headers */
	if (elf_getphdrnum(hdl->elf_desc, &hdl->phdrnum) != 0) {
		mppa_dl_errno(E_ELF_PHDRNUM);
		return (void*)hdl;
	}

	/* allocate memory to load needed ELF segments */
	hdl->mem_addr = memalign (mppa_dl_load_segments_align(hdl),
				  mppa_dl_load_segments_memsz(hdl));
	if (hdl->mem_addr == NULL) {
		mppa_dl_errno(E_MEM_ALIGN);
		return (void*)hdl;
	}

	/* iterate program headers to load PT_LOAD segments */
	for (i = 0; i < (int)hdl->phdrnum; i++) {
		if (gelf_getphdr( hdl->elf_desc, (int)i, &phdr ) == NULL) {
			mppa_dl_errno(E_ELF_PHDR);
			return (void*)hdl;
		}

		switch (phdr.p_type) {
		case PT_LOAD: /* load segment to memory */
			memcpy(hdl->mem_addr + phdr.p_vaddr,
			       image + phdr.p_offset, phdr.p_memsz);
			break;
		default: /* do not load other segments */
			break;
		}
	}

	return (void*)hdl;
}

int mppa_dl_unload(void *handle)
{
	int ret = 0;

	if (elf_end(((mppa_dl_handle_t*)handle)->elf_desc) != 0) {
		mppa_dl_errno(E_ELF_END);
		ret = -1;
	}

	free(((mppa_dl_handle_t*)handle)->mem_addr);
	free(handle);

	return ret;
}
