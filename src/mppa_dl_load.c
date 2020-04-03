/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_load.h"
#include "mppa_dl_mem.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl)
{
	MPPA_DL_LOG(1, "- mppa_dl_load_addr()\n");
	return hdl->addr;
}


enum MPPA_DL_ERRNO mppa_dl_init_handle(mppa_dl_handle_t *hdl, ElfKVX_Dyn *dyn,
			void *off, mppa_dl_handle_t *parent,
			int availability)
{
	enum MPPA_DL_ERRNO ret = E_NONE;
	MPPA_DL_LOG(1, "> mppa_dl_init_handle(%p, %p, %p, %p, %d)\n",
		    hdl, dyn, off, parent, availability);

	size_t k = 0, relasz = 0, pltrelsz = 0, name_ofs = 0;
	dyn = (ElfKVX_Dyn *)((ElfKVX_Addr)dyn + (ElfKVX_Addr)off);

	hdl->addr = (ElfKVX_Addr*)off;
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
	hdl->name = NULL;

	mppa_dl_debug_init_debug(hdl);

	if (parent != NULL)
		parent->child = hdl;

	while (dyn[k].d_tag != DT_NULL) {
		switch (dyn[k].d_tag) {
		case DT_HASH:
			hdl->hash = (ElfKVX_Word *)
					(dyn[k].d_un.d_ptr + (ElfKVX_Addr)off);
			break;
		case DT_STRTAB:
			hdl->strtab = (char *)
					(dyn[k].d_un.d_ptr + (ElfKVX_Addr)off);
			break;
		case DT_STRSZ:
			hdl->strsz = dyn[k].d_un.d_val;
			break;
		case DT_SYMTAB:
			hdl->symtab = (ElfKVX_Sym *)
					(dyn[k].d_un.d_ptr + (ElfKVX_Addr)off);
			break;
		case DT_RELA:
			hdl->rela = (ElfKVX_Rela *)
					(dyn[k].d_un.d_ptr + (ElfKVX_Addr)off);
			break;
		case DT_RELASZ:
			relasz = dyn[k].d_un.d_val;
			break;
		case DT_JMPREL:
			hdl->jmprel = (void *)
					(dyn[k].d_un.d_ptr + (ElfKVX_Addr)off);
			break;
		case DT_PLTREL:
			hdl->pltrel = dyn[k].d_un.d_val;
			break;
		case DT_PLTRELSZ:
			pltrelsz = dyn[k].d_un.d_val;
			break;
		case DT_SONAME:
			name_ofs = dyn[k].d_un.d_val;
			break;
		case DT_INIT_ARRAY:
		case DT_FINI_ARRAY:
		case DT_INIT:
		case DT_FINI:
		  /* Only reject for object being dynamically
		     loaded. Don't reject handle creation for the main
		     program context */
		  if (parent != NULL) {
		    ret = E_UNSUP_CTOR_DTOR;
		    goto early_exit;
		  }
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

	if (name_ofs && hdl->strtab) {
		int name_len = strlen(hdl->strtab + name_ofs) + 1;
		hdl->name = mppa_dl_malloc(name_len);
		strcpy(hdl->name, hdl->strtab + name_ofs);
	}

	hdl->relan = relasz / sizeof(ElfKVX_Rela);

	if (hdl->pltrel == DT_RELA || hdl->pltrel == DT_NULL)
		hdl->pltreln = pltrelsz / sizeof(ElfKVX_Rela);
	else
		ret = E_INIT_HDL;

	early_exit:
	MPPA_DL_LOG(1, "< mppa_dl_init_handle(%p, %p, %p, %p, %d)\n",
		    hdl, dyn, off, parent, availability);

	return ret;
}
