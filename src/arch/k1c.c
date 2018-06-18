#ifndef __K1C__
#error "This file is K1C/Coolidge specific"
#endif

#include "mppa_dl_load.h"

/* Disable warning: the casting from/to an int to/from a pointer
 * always triggers the error. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
int mppa_dl_apply_rela(mppa_dl_handle_t *hdl, ElfK1_Rela rel)
{
	MPPA_DL_LOG(1, "> mppa_dl_apply_rela(%p, rel=0x%lx,0x%lx,0x%lx)\n",
		    hdl, (long)rel.r_info, (long)rel.r_offset, (long)rel.r_addend);

	ElfK1_Word tmp;
	ElfK1_Lword tmp64;

	ElfK1_Sym sym = hdl->symtab[ELFK1_R_SYM(rel.r_info)];

	MPPA_DL_LOG(2, ">> relocation at 0x%lx, of type %ld, "
		    "with symbol '%s', and addend->0x%lx\n",
		    rel.r_offset, (long int)ELFK1_R_TYPE(rel.r_info),
		    &hdl->strtab[sym.st_name], rel.r_addend);

	switch (ELFK1_R_TYPE(rel.r_info)) {
	case R_K1_NONE:
		break;
	case R_K1_32:
	case R_K1_RELATIVE:
	        tmp = ((ElfK1_Word)rel.r_addend + (uintptr_t) hdl->addr);
		memcpy((void*)((uintptr_t)hdl->addr + rel.r_offset), &tmp,
			sizeof(ElfK1_Word));
		MPPA_DL_LOG(2, ">> Wrote 0x%lx at 0x%lx\n",
			    (long unsigned)tmp, (long unsigned) ((uintptr_t)hdl->addr + rel.r_offset));

		break;
	case R_K1_GLOB_DAT:
	case R_K1_JMP_SLOT:
	  tmp = (uintptr_t) mppa_dl_sym_lookup(hdl, &hdl->strtab[sym.st_name], 0);
		if (tmp == 0 && mppa_dl_errno_get_status() != E_NONE)
			return -2;

		memcpy((void *)hdl->addr + rel.r_offset, &tmp,
			sizeof(ElfK1_Word));
		break;

	case R_K1_64:
	case R_K1_RELATIVE64:
	        tmp64 = ((ElfK1_Lword)rel.r_addend + (uintptr_t) hdl->addr);
		memcpy((void*)((uintptr_t)hdl->addr + rel.r_offset), &tmp64,
			sizeof(ElfK1_Lword));
		MPPA_DL_LOG(2, ">> Wrote 0x%lx at 0x%lx\n",
			    (long unsigned)tmp64, (long unsigned) ((uintptr_t)hdl->addr + rel.r_offset));

		break;


	default:
		MPPA_DL_LOG(2, ">> Unhandled relocation type %ld\n", rel.r_info);
		return -1;
	}

	MPPA_DL_LOG(1, "< mppa_dl_apply_rela(%p, rel=0x%lx,0x%lx,0x%lx)\n",
		    hdl, (long)rel.r_info, (long)rel.r_offset, (long)rel.r_addend);

	return 0;
}
