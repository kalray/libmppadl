/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl)
{
	return hdl->mem_addr;
}

void *mppa_dl_load(const char *image, size_t size)
{
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

	/* process the ELF image present in memory */
	hdl->elf_desc = elf_memory((char*)image, size);
	if (hdl->elf_desc == NULL) {
		mppa_dl_errno(E_ELF_MEM);
		return NULL;
	}

	/* retrieve the number of EFL program headers */
	if (elf_getphdrnum(hdl->elf_desc, &hdl->phdrnum) != 0) {
		mppa_dl_errno(E_ELF_PHDRNUM);
		return NULL;
	}

	/* allocate memory to load needed ELF segments */
	hdl->mem_addr = memalign (mppa_dl_load_segments_align(hdl),
				  mppa_dl_load_segments_memsz(hdl));
	if (hdl->mem_addr == NULL) {
		mppa_dl_errno(E_MEM_ALIGN);
		return NULL;
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

	//DEBUG
	GElf_Shdr shdrstr;
	size_t ndx;
	elf_getshdrstrndx(hdl->elf_desc, &ndx);
	gelf_getshdr(elf_getscn(hdl->elf_desc, ndx), &shdrstr);
	//GUBED

	/* get the relocations and symbol table sections */
	while ((scn = elf_nextscn(hdl->elf_desc, scn)) != NULL) {
		if (gelf_getshdr(scn, &shdr) == NULL) {
			mppa_dl_errno(E_ELF_SHDR);
			return NULL;
		}

		Elf_Data* data = NULL;
		GElf_Dyn dyn;
		int d;
		//DEBUG
		printf("---examine section %s\n",
		       image + shdrstr.sh_offset + shdr.sh_name);
		//GUBED

		switch (shdr.sh_type) {
		case SHT_RELA:
			mppa_dl_add_relascn(hdl, scn);
			break;
		case SHT_SYMTAB:
			hdl->s_symtab = scn;
			break;
		case SHT_STRTAB:
			hdl->s_strtab = scn;
			break;
		case SHT_DYNSYM:
			hdl->s_dynsym = scn;
			break;
		case SHT_DYNAMIC:
			d = 0;
			while (gelf_getdyn(elf_getdata(scn, data) ,d, &dyn)
			       != NULL) { //iterate also sur getdata?
				//DEBUG
				printf("--dynamic object tag->%lld\n",
				       dyn.d_tag);
				//GUBED
				switch (dyn.d_tag) {
				case DT_STRTAB:
					hdl->strtaboff = dyn.d_un.d_ptr;
					break;
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
