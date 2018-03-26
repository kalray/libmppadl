#ifndef __K1B__
#error "This file is K1B/Bostan specific"
#endif

#include "mppa_dl_load.h"


int mppa_dl_apply_rela(mppa_dl_handle_t *hdl, ElfK1_Rela rel)
{
	MPPA_DL_LOG(1, "> mppa_dl_apply_rela()\n");

	ElfK1_Word tmp;
	ElfK1_Sym sym = hdl->symtab[ELFK1_R_SYM(rel.r_info)];

	MPPA_DL_LOG(2, ">> relocation at %lx, of type %d, "
		    "with symbol '%s', and addend->%lx\n",
		    rel.r_offset, ELFK1_R_TYPE(rel.r_info),
		    &hdl->strtab[sym.st_name], rel.r_addend);

	switch (ELFK1_R_TYPE(rel.r_info)) {
	case R_K1_32:
	case R_K1_RELATIVE:
		tmp = ((ElfK1_Word)rel.r_addend + (ElfK1_Word)hdl->addr);
		memcpy((void *)hdl->addr + rel.r_offset, &tmp,
			sizeof(ElfK1_Word));
		break;
	case R_K1_GLOB_DAT:
	case R_K1_JMP_SLOT:
		tmp = (ElfK1_Word)(ElfK1_Word*)
			mppa_dl_sym_lookup(hdl, &hdl->strtab[sym.st_name], 0);

		if (tmp == 0 && mppa_dl_errno_get_status() != E_NONE)
			return -2;

		memcpy((void *)hdl->addr + rel.r_offset, &tmp,
			sizeof(ElfK1_Word));
		break;
	default:
		return -1;
	}

	MPPA_DL_LOG(1, "< mppa_dl_apply_rela()\n");

	return 0;
}
