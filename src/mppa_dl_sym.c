/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_sym.h"

void *mppa_dl_sym_lookup2(mppa_dl_handle_t *hdl, const char *symbol)
{
	MPPA_DL_LOG(1, "> mppa_dl_sym_lookup2()\n");

	ElfK1_Sym sym;
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
			MPPA_DL_LOG(1, "< mppa_dl_sym_lookup2()\n");
			return NULL;
		}
		sym = hdl->symtab[symndx];
	}

	/* ELF is a shared object file, so sym.st_value holds the symbol address */
	if (sym.st_shndx != SHN_UNDEF && hdl->type == ET_DYN) {
		MPPA_DL_LOG(2, ">> symbol found at offset 0x%lx\n",
			    sym.st_value);
		MPPA_DL_LOG(1, "< mppa_dl_sym_lookup2()\n");

		return (ElfK1_Addr *)((ElfK1_Addr)hdl->addr + sym.st_value);
	}

	MPPA_DL_LOG(1, "< mppa_dl_sym_lookup2()\n");

	mppa_dl_errno(E_NO_SYM);
	return NULL;
}


void *mppa_dl_sym_lookup(mppa_dl_handle_t *hdl, const char* symbol, int local)
{
	MPPA_DL_LOG(1, "> mppa_dl_sym_lookup()\n");

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

	MPPA_DL_LOG(1, "< mppa_dl_sym_lookup()\n");

	return NULL;
}
