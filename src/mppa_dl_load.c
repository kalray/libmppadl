/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_load.h"

size_t mppa_dl_load_segments_memsz(mppa_dl_handle_t *hdl)
{
	size_t memsz = 0;
	int i;
	GElf_Phdr phdr;

	/* iterate program headers, in descending order, then compute the
	   required memory size to load segments */
	for (i = (int)hdl->phdrnum - 1; i >= 0; i--) {
		if (gelf_getphdr(hdl->elf_desc, i, &phdr) == NULL) {
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
	for (i = 0; i < (int)hdl->phdrnum; i++) {
		if (gelf_getphdr(hdl->elf_desc, i, &phdr) == NULL) {
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
	if (hdl->s_reloc != NULL) {
		mppa_dl_shdr_t *tmp = hdl->s_reloc;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		tmp->next = (mppa_dl_shdr_t*)malloc(sizeof(mppa_dl_shdr_t));
		tmp->next->scn = scn;
		tmp->next->next = NULL;
	} else {
		hdl->s_reloc = (mppa_dl_shdr_t*)malloc(sizeof(mppa_dl_shdr_t));
		hdl->s_reloc->scn = scn;
		hdl->s_reloc->next = NULL;
	}

	return 0;
}

int mppa_dl_apply_reloc(mppa_dl_handle_t *hdl)
{
	int i;
	GElf_Rela rela;
	GElf_Sym sym;
	void *buffer = NULL;
	unsigned tmp;

	mppa_dl_shdr_t *relocs = hdl->s_reloc;
	while (relocs != NULL) {
		for (i = 0; gelf_getrela(elf_getdata(relocs->scn, NULL), i, &rela); i++) {

			printf("-reloc offset->%llx type->%llu sym->%llu addend->%llx\n",
			       rela.r_offset,
			       GELF_R_TYPE(rela.r_info),
			       GELF_R_SYM(rela.r_info),
			       rela.r_addend);


			switch (GELF_R_TYPE(rela.r_info)) {
			case R_K1_RELATIVE:
				/* add r_addend to the value located at r_offset */
				buffer = malloc(sizeof(unsigned));
				memcpy(buffer, hdl->mem_addr + rela.r_offset, sizeof(unsigned));
				tmp = (*(unsigned*)buffer + (unsigned)rela.r_addend);
				memcpy(hdl->mem_addr + rela.r_offset, &tmp, sizeof(unsigned));
				free(buffer);
				break;
			default:
				mppa_dl_errno(E_UNKN_RELOC);
				return -1;
			}



			if (gelf_getsym(elf_getdata(hdl->s_dynsym, NULL), GELF_R_SYM(rela.r_info), &sym) == NULL) {
				mppa_dl_errno(E_ELF_SYM);
				return -1;
			}
			else {

				printf("-symbol(.dynsym) %s, name->%lu shndx->%d value->%llu info->%d size->%llu\n",
				       (char*)hdl->mem_addr + hdl->strtaboff + sym.st_name,
				       sym.st_name, sym.st_shndx, sym.st_value, sym.st_info, sym.st_size);
			}

			if (gelf_getsym(elf_getdata(hdl->s_symtab, NULL), GELF_R_SYM(rela.r_info), &sym) == NULL) {
				mppa_dl_errno(E_ELF_SYM);
				return -1;
			}
			else {

				printf("-symbol(.symtab) %s, name->%lu shndx->%d value->%llu info->%d size->%llu\n",
				       (char*)hdl->mem_addr + hdl->strtaboff + sym.st_name,
				       sym.st_name, sym.st_shndx, sym.st_value, sym.st_info, sym.st_size);
			}

		}
		relocs = relocs->next;
	}

	return 0;
}

//printf("%llu %s\n", shdr.sh_offset, &((char*)hdl->mem_addr + dyn.d_un.d_ptr)[14]);
