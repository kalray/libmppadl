/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl.h"

void *mppa_dl_load(const char *image, size_t size)
{
	__mppa_dl_loglevel = 0;

	if (__mppa_dl_loglevel > 0)
		fprintf(stderr, "--> mppa_dl_load\n");

	mppa_dl_errno(E_NONE);
	mppa_dl_handle_t *hdl = NULL;
	GElf_Phdr phdr;
	GElf_Shdr shdr;
	Elf_Scn *scn = NULL;
	int i;

	/* querying the current operating version of the ELF library */
	if (elf_version(EV_CURRENT) == EV_NONE) {
		mppa_dl_errno(E_ELF_OLD);
		return (void*)hdl;
	}

	hdl = (mppa_dl_handle_t*)malloc(sizeof(mppa_dl_handle_t));
	hdl->hdl_elf = NULL;
	hdl->hdl_phdrnum = 0;
	hdl->hdl_addr = NULL;
	hdl->hdl_strtab = 0;
	hdl->hdl_dynsym = NULL;
	hdl->hdl_reloc_l = NULL;

	/* process the ELF image present in memory */
	hdl->hdl_elf = elf_memory((char*)image, size);
	if (hdl->hdl_elf == NULL) {
		mppa_dl_errno(E_ELF_MEM);
		return NULL;
	}

	/* retrieve the number of EFL program headers */
	if (elf_getphdrnum(hdl->hdl_elf, &hdl->hdl_phdrnum) != 0) {
		mppa_dl_errno(E_ELF_PHDRNUM);
		return NULL;
	}

	/* allocate memory to load needed ELF segments */
	hdl->hdl_addr = memalign (mppa_dl_load_segments_align(hdl),
				  mppa_dl_load_segments_memsz(hdl));
	if (hdl->hdl_addr == NULL) {
		mppa_dl_errno(E_MEM_ALIGN);
		return NULL;
	}

	/* iterate program headers to load PT_LOAD segments */
	for (i = 0; i < (int)hdl->hdl_phdrnum; i++) {
		if (gelf_getphdr( hdl->hdl_elf, (int)i, &phdr ) == NULL) {
			mppa_dl_errno(E_ELF_PHDR);
			return (void*)hdl;
		}

		switch (phdr.p_type) {
		case PT_LOAD: /* load segment to memory */
			memcpy(hdl->hdl_addr + phdr.p_vaddr,
			       image + phdr.p_offset, phdr.p_memsz);
			break;
		default: /* do not load other segments */
			break;
		}
	}

	GElf_Shdr shdrstr;
	size_t ndx;
	if (__mppa_dl_loglevel == 2) {
		elf_getshdrstrndx(hdl->hdl_elf, &ndx);
		gelf_getshdr(elf_getscn(hdl->hdl_elf, ndx), &shdrstr);
	}

	/* get the relocations and symbol table sections */
	while ((scn = elf_nextscn(hdl->hdl_elf, scn)) != NULL) {
		if (gelf_getshdr(scn, &shdr) == NULL) {
			mppa_dl_errno(E_ELF_SHDR);
			return NULL;
		}

		GElf_Dyn dyn;
		int d;
		if (__mppa_dl_loglevel == 2)
			fprintf(stderr, "---examine section %s\n",
				image + shdrstr.sh_offset + shdr.sh_name);

		switch (shdr.sh_type) {
		case SHT_RELA:
			if (__mppa_dl_loglevel == 2)
				fprintf(stderr, "---add a relation section in "
					"the handle's list\n");
			mppa_dl_add_relascn(hdl, scn);
			break;
		case SHT_DYNSYM:
			hdl->hdl_dynsym = scn;
			break;
		case SHT_DYNAMIC:
			d = 0;
			while (gelf_getdyn(elf_getdata(scn, NULL) ,d, &dyn)
			       != NULL) {
				if (__mppa_dl_loglevel == 2)
					fprintf(stderr,
						"--dynamic object tag->%lld\n",
						dyn.d_tag);

				switch (dyn.d_tag) {
				case DT_STRTAB:
					hdl->hdl_strtab = dyn.d_un.d_ptr;
					break;
					/*case DT_HASH:
					  hdl->hashoff = dyn.d_un.d_ptr;
					  break;*/
				default:
					break;
				}
				d++;
			}
			break;
		default:
			break;
		}
	}

	if (mppa_dl_apply_reloc(hdl) == -1)
		return NULL;

	if (__mppa_dl_loglevel > 0)
		fprintf(stderr, "<-- mppa_dl_load\n");
	return (void*)hdl;
}

void *mppa_dl_sym(void *handle, const char* symbol)
{
	return mppa_dl_sym_lookup((mppa_dl_handle_t*)handle, symbol);
}

int mppa_dl_unload(void *handle)
{
	if (__mppa_dl_loglevel > 0)
		fprintf(stderr, "--> mppa_dl_unload\n");

	int ret = 0;

	if (elf_end(((mppa_dl_handle_t*)handle)->hdl_elf) != 0) {
		mppa_dl_errno(E_ELF_END);
		ret = -1;
	}

	mppa_dl_shdr_t *tmp, *rlc = ((mppa_dl_handle_t*)handle)->hdl_reloc_l;
	while ( rlc != NULL) {
		tmp = rlc->next;
		free(rlc);
		rlc = tmp;
	}

	free(((mppa_dl_handle_t*)handle)->hdl_addr);
	free(handle);

	if (__mppa_dl_loglevel > 0)
		fprintf(stderr, "<-- mppa_dl_unload\n");

	return ret;
}
