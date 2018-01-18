/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#include "mppa_dl_load.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl)
{
	return hdl->hdl_addr;
}

size_t mppa_dl_load_segments_memsz(mppa_dl_handle_t *hdl)
{
	size_t memsz = 0;
	int i;
	GElf_Phdr phdr;

	/* iterate program headers, in descending order, then compute the
	   required memory size to load segments */
	for (i = (int)hdl->hdl_phdrnum - 1; i >= 0; i--) {
		if (gelf_getphdr(hdl->hdl_elf, i, &phdr) == NULL) {
			mppa_dl_errno(E_ELF_PHDR);
			return memsz;
		}

		if (phdr.p_type == PT_LOAD) {
			memsz = phdr.p_vaddr + phdr.p_memsz;
			i = -1;
		}
	}

	return memsz;
}

size_t mppa_dl_load_segments_align(mppa_dl_handle_t *hdl)
{
	size_t malign = 0;
	int i;
	GElf_Phdr phdr;

	/* iterate program headers, in ascending order, then compute the
	   maximum alignement required to load segments */
	for (i = 0; i < (int)hdl->hdl_phdrnum; i++) {
		if (gelf_getphdr(hdl->hdl_elf, i, &phdr) == NULL) {
			mppa_dl_errno(E_ELF_PHDR);
			return malign;
		}

		if (phdr.p_type == PT_LOAD)
			malign = malign >= phdr.p_align ? malign : phdr.p_align;
	}

	return malign;
}

int mppa_dl_add_relascn(mppa_dl_handle_t *hdl, Elf_Scn *scn)
{
	if (hdl->hdl_reloc_l != NULL) {
		mppa_dl_shdr_t *tmp = hdl->hdl_reloc_l;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		tmp->next = (mppa_dl_shdr_t*)malloc(sizeof(mppa_dl_shdr_t));
		tmp->next->scn = scn;
		tmp->next->next = NULL;
	} else {
		hdl->hdl_reloc_l =
			(mppa_dl_shdr_t*)malloc(sizeof(mppa_dl_shdr_t));
		hdl->hdl_reloc_l->scn = scn;
		hdl->hdl_reloc_l->next = NULL;
	}

	return 0;
}

int mppa_dl_apply_reloc(mppa_dl_handle_t *hdl)
{
	if (__mppa_dl_loglevel > 0)
		fprintf(stderr, "--> mppa_dl_apply_reloc\n");

	int i;
	GElf_Rela rela;
	GElf_Sym sym;
	unsigned tmp;

	mppa_dl_shdr_t *relocs = hdl->hdl_reloc_l;
	while (relocs != NULL) {
		if (__mppa_dl_loglevel == 2)
			fprintf(stderr, "--relocation section:\n");

		for (i = 0;
		     gelf_getrela(elf_getdata(relocs->scn, NULL), i, &rela);
		     i++) {

			if (__mppa_dl_loglevel == 2)
				fprintf(stderr,
					"-reloc offset->%llx type->%llu"
					"sym->%llu addend->%llx\n",
					rela.r_offset,
					GELF_R_TYPE(rela.r_info),
					GELF_R_SYM(rela.r_info),
					rela.r_addend);


			switch (GELF_R_TYPE(rela.r_info)) {
			case R_K1_RELATIVE:
				tmp = ((unsigned)rela.r_addend
				       + (unsigned)hdl->hdl_addr);
				memcpy(hdl->hdl_addr + rela.r_offset, &tmp,
				       sizeof(unsigned));
				break;
			default:
				mppa_dl_errno(E_UNKN_RELOC);
				return -1;
			}



			if (gelf_getsym(elf_getdata(hdl->hdl_dynsym, NULL),
					GELF_R_SYM(rela.r_info),
					&sym) == NULL) {
				mppa_dl_errno(E_ELF_SYM);
				return -1;
			}
			else {
				if (__mppa_dl_loglevel == 2)
					fprintf(stderr, "-symbol(.dynsym) %s, "
						"name->%lu shndx->%d "
						"value->%llu info->%d "
						"size->%llu\n",
						(char*)hdl->hdl_addr +
						hdl->hdl_strtab + sym.st_name,
						sym.st_name, sym.st_shndx,
						sym.st_value, sym.st_info,
						sym.st_size);
			}
		}
		relocs = relocs->next;
	}

	if (__mppa_dl_loglevel > 0)
		fprintf(stderr, "<-- mppa_dl_apply_reloc\n");

	return 0;
}
