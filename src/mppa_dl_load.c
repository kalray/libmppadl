/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_load.h"

void *mppa_dl_load_addr(mppa_dl_handle_t *hdl)
{
#if VERBOSE > 0
	fprintf(stderr, "- mppa_dl_load_addr()\n");
#endif
	return hdl->addr;
}


int mppa_dl_init_handle(mppa_dl_handle_t *hdl, ElfK1_Dyn *dyn,
			void *off, mppa_dl_handle_t *parent)
{
#if VERBOSE > 0
	fprintf(stderr, "> mppa_dl_init_handle()\n");
#endif
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
		hdl->pltreln = pltrelsz / relaent;
	else
		return -1;

#if VERBOSE > 0
	fprintf(stderr, "< mppa_dl_init_handle()\n");
#endif
	return 0;
}


int mppa_dl_apply_rela(mppa_dl_handle_t *hdl, ElfK1_Rela rel)
{
#if VERBOSE > 0
	fprintf(stderr, "> mppa_dl_apply_rela()\n");
#endif
	ElfK1_Word tmp;
	ElfK1_Sym sym = hdl->symtab[ELFK1_R_SYM(rel.r_info)];

#if VERBOSE > 1
	fprintf(stderr,	">> relocation at %lx, of type %d, "
		"with symbol '%s', and addend->%lx\n",
		rel.r_offset, ELFK1_R_TYPE(rel.r_info),
		&hdl->strtab[sym.st_name], rel.r_addend);
#endif

	switch (ELFK1_R_TYPE(rel.r_info)) {
	case R_K1_RELATIVE:
		tmp = ((ElfK1_Word)rel.r_addend + (ElfK1_Word)hdl->addr);
		memcpy((void *)hdl->addr + rel.r_offset, &tmp,
			sizeof(ElfK1_Word));
		break;
	case R_K1_GLOB_DAT:
	case R_K1_JMP_SLOT:
		tmp = (ElfK1_Word)mppa_dl_sym_lookup(hdl,
			&hdl->strtab[sym.st_name]);

		if (tmp == 0 && mppa_dl_errno_get_status() != E_NONE)
			return -2;

		memcpy((void *)hdl->addr + rel.r_offset, &tmp,
			sizeof(ElfK1_Word));
		break;
	default:
		return -1;
	}

#if VERBOSE > 0
	fprintf(stderr, "< mppa_dl_apply_rela()\n");
#endif

	return 0;
}
