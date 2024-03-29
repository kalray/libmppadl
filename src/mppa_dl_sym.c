/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_sym.h"

void *mppa_dl_sym_lookup2(mppa_dl_handle_t *hdl, const char *symbol)
{
	MPPA_DL_LOG(1, "> mppa_dl_sym_lookup2(%p, %s)\n",
		    hdl, symbol);

	ElfKVX_Sym sym;
	size_t bndx;
	size_t symndx;

	/* bucket index */
	bndx = elf_hash(symbol) % hdl->nbucket;

	if (hdl->nchain < bndx || hdl->nbucket < bndx) {
		mppa_dl_errno(E_SYM_OUT);
		return NULL;
	}

	/* symbol index is given by the bucket table */
	symndx = hdl->bucket[bndx];
	sym = hdl->symtab[symndx];

	/* check if symbol pointed by symndx is equal to needed symbol */
	while (strcmp(symbol, &hdl->strtab[sym.st_name]) != 0) {
		/* update symbol index with chain table */
		symndx = hdl->chain[symndx];
		if (symndx == 0) {
			mppa_dl_errno(E_END_CHAIN);
			MPPA_DL_LOG(1, "< mppa_dl_sym_lookup2(%p, %s) -> NULL\n",
				    hdl, symbol);
			return NULL;
		}
		sym = hdl->symtab[symndx];
	}

	/* ELF is a shared object file, so sym.st_value holds the symbol address */
	if (sym.st_shndx != SHN_UNDEF && hdl->type == ET_DYN) {
		MPPA_DL_LOG(2, ">> symbol found at offset 0x%lx\n",
			    sym.st_value);
		ElfKVX_Addr ret = ((ElfKVX_Addr)hdl->addr + sym.st_value);
		MPPA_DL_LOG(1, "< mppa_dl_sym_lookup2(%p, %s) -> %lx\n",
			    hdl, symbol, (long)ret);

		return (void*)ret;
	}

	MPPA_DL_LOG(1, "< mppa_dl_sym_lookup2(%p, %s) -> NO_SYM\n",
		    hdl, symbol);

	mppa_dl_errno(E_NO_SYM);
	return NULL;
}


void *mppa_dl_sym_lookup(mppa_dl_handle_t *hdl, const char* symbol, int local)
{
	MPPA_DL_LOG(1, "> mppa_dl_sym_lookup(%p, %s, local=%d)\n",
		    hdl, symbol, local);

	void *sym;
	mppa_dl_handle_t *lookat;

	/* search symbol globally */
	if (local == 0) {

		/* symbol search start at first parent of the current handle. */
		lookat = hdl->parent;

		/* look in the tail of handle list */
		while (lookat != NULL) {
			if (lookat->availability == MPPA_DL_GLOBAL) {
				sym = mppa_dl_sym_lookup2(lookat, symbol);

				if (sym != NULL || 
				    (sym == NULL &&
				     mppa_dl_errno_get_status() == E_NONE)) {
					MPPA_DL_LOG(1,
						    "< mppa_dl_sym_lookup()\n");
					return sym;
				} else {
					lookat = lookat->parent;
				}
			} else {
				lookat = lookat->parent;
			}
		}
	}

	/* look in the head of the handle list (or local search only) */
	sym = mppa_dl_sym_lookup2(hdl, symbol);
	if (sym != NULL ||
	    (sym == NULL && mppa_dl_errno_get_status() == E_NONE))
		return sym;

	MPPA_DL_LOG(1, "< mppa_dl_sym_lookup(%p, %s, local=%d)\n",
		    hdl, symbol, local);

	return NULL;
}

void *mppa_dl_sym_lookup_local(struct mppa_dl_handle *hdl,
	const char *sym_name)
{
	ElfKVX_Sym *sym;
	unsigned int i;

	if (hdl->sht_strtab == NULL || hdl->sht_symtab_syms == NULL)
		return NULL;

	for (i = 0; i < hdl->sht_symtab_nb_syms; i++) {
		sym = &hdl->sht_symtab_syms[i];
		if (!strcmp(sym_name, &hdl->sht_strtab[sym->st_name]))
			return hdl->addr + sym->st_value;
	}

	return NULL;
}
