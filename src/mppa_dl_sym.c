/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#include "mppa_dl_sym.h"

void *mppa_dl_sym_lookup(mppa_dl_handle_t *hdl, const char *symbol)
{
	unsigned long tmp, hash = elf_hash(symbol);
	int i;
	GElf_Sym sym;
	GElf_Shdr ss;

	/* iterate the (dynamic) symbol table */
	for (i = 0;
	     gelf_getsym(elf_getdata(hdl->hdl_dynsym, NULL), i, &sym) != NULL;
	     i++) {

		if (__mppa_dl_loglevel == 2)
			fprintf(stderr, "-symbol(.dynsym) %s, name->%lu "
				"shndx->%d value->%llu info->%d "
				"(type->%d) size->%llu\n",
				(char*)hdl->hdl_addr + hdl->hdl_strtab
				+ sym.st_name,
				sym.st_name, sym.st_shndx, sym.st_value,
				sym.st_info, GELF_ST_TYPE(sym.st_info),
				sym.st_size);

		tmp = elf_hash((char*)hdl->hdl_addr +
			       hdl->hdl_strtab + sym.st_name);
		if (tmp == hash) {
			if (gelf_getshdr(elf_getscn(hdl->hdl_elf, sym.st_shndx),
					 &ss) == NULL) {
				mppa_dl_errno(E_ELF_SHDR);
				return NULL;
			} else {
				return hdl->hdl_addr + ss.sh_addr;
			}
		}
	}

	return NULL;
}
