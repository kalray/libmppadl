/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_load.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl)
{
	MPPA_DL_LOG(1, "- mppa_dl_load_addr()\n");
	return hdl->addr;
}


int mppa_dl_init_handle(mppa_dl_handle_t *hdl, ElfK1_Dyn *dyn,
			void *off, mppa_dl_handle_t *parent,
			int availability)
{
	MPPA_DL_LOG(1, "> mppa_dl_init_handle()\n");

	size_t k = 0, relasz = 0, relaent = 0, pltrelsz = 0;
	dyn = (ElfK1_Dyn *)((ElfK1_Addr)dyn + (ElfK1_Addr)off);

	hdl->addr = (ElfK1_Addr*)off;
	hdl->type = ET_DYN;
	hdl->nbucket = 0;
	hdl->nchain = 0;
	hdl->bucket = NULL;
	hdl->chain = NULL;
	hdl->hash = NULL;
	hdl->rela = NULL;
	hdl->relan = 0;
	hdl->jmprel = NULL;
	hdl->pltrel = DT_NULL;
	hdl->pltreln = 0;
	hdl->strtab = 0;
	hdl->strsz = 0;
	hdl->symtab = 0;
	hdl->parent = parent;
	hdl->child = NULL;
	hdl->availability = availability;

	if (parent != NULL)
		parent->child = hdl;

	while (dyn[k].d_tag != DT_NULL) {
		switch (dyn[k].d_tag) {
		case DT_HASH:
			hdl->hash = (ElfK1_Addr *)
					(dyn[k].d_un.d_ptr + (ElfK1_Addr)off);
			break;
		case DT_STRTAB:
			hdl->strtab = (char *)
					(dyn[k].d_un.d_ptr + (ElfK1_Addr)off);
			break;
		case DT_STRSZ:
			hdl->strsz = dyn[k].d_un.d_val;
			break;
		case DT_SYMTAB:
			hdl->symtab = (ElfK1_Sym *)
					(dyn[k].d_un.d_ptr + (ElfK1_Addr)off);
			break;
		case DT_RELA:
			hdl->rela = (ElfK1_Rela *)
					(dyn[k].d_un.d_ptr + (ElfK1_Addr)off);
			break;
		case DT_RELASZ:
			relasz = dyn[k].d_un.d_val;
			break;
		case DT_JMPREL:
			hdl->jmprel = (void *)
					(dyn[k].d_un.d_ptr + (ElfK1_Addr)off);
			break;
		case DT_PLTREL:
			hdl->pltrel = dyn[k].d_un.d_val;
			break;
		case DT_PLTRELSZ:
			pltrelsz = dyn[k].d_un.d_val;
			break;
		case DT_RELAENT:
			relaent = dyn[k].d_un.d_val;
			break;
		default:
			break;
		}
		k++;
	}

	hdl->nbucket = hdl->hash[0];
	hdl->nchain = hdl->hash[1];
	hdl->bucket = &hdl->hash[2];
	hdl->chain = &hdl->hash[2+hdl->nbucket];

	if (relasz == 0 || relaent == 0)
		hdl->relan = 0;
	else
		hdl->relan = relasz / relaent;

	if (hdl->pltrel == DT_RELA || hdl->pltrel == DT_NULL)
		if (pltrelsz == 0 || relaent == 0)
			hdl->pltreln = 0;
		else
			hdl->pltreln = pltrelsz / relaent;
	else
		return -1;

	MPPA_DL_LOG(1, "< mppa_dl_init_handle()\n");

	return 0;
}
