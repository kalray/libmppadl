#ifndef __KV3__
#error "This file is KV3/Coolidge specific"
#endif

#include "mppa_dl_load.h"

/* Macros used for relocations that depends on target mode. */
#ifdef __KV3_64__
#define R_TMP tmp64
#define R_EWORD ElfKVX_Lword
#else
#define R_TMP tmp
#define R_EWORD ElfKVX_Word
#endif

/* Disable warning: the casting from/to an int to/from a pointer
 * always triggers the error. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
int mppa_dl_apply_rela(mppa_dl_handle_t *hdl, ElfKVX_Rela rel)
{
	MPPA_DL_LOG(1, "> mppa_dl_apply_rela(%p, rel=0x%lx,0x%lx,0x%lx)\n",
		    hdl, (long)rel.r_info, (long)rel.r_offset, (long)rel.r_addend);

	ElfKVX_Word tmp;
	ElfKVX_Lword tmp64;

	ElfKVX_Sym sym = hdl->symtab[ELFKVX_R_SYM(rel.r_info)];

	MPPA_DL_LOG(2, ">> relocation at 0x%lx, of type %ld, "
		    "with symbol '%s', and addend->0x%lx\n",
		    rel.r_offset, (long int)ELFKVX_R_TYPE(rel.r_info),
		    &hdl->strtab[sym.st_name], rel.r_addend);

	switch (ELFKVX_R_TYPE(rel.r_info)) {
	case R_KVX_NONE:
		break;

	case R_KVX_32:
		tmp = (uintptr_t) mppa_dl_sym_lookup(hdl, &hdl->strtab[sym.st_name], 0);
		if (tmp == 0 && mppa_dl_errno_get_status() != E_NONE
		    && ELFKVX_ST_BIND(sym.st_info) != STB_WEAK)
			return -2;
		tmp += (ElfKVX_Word)rel.r_addend;
		memcpy((void *)hdl->addr + rel.r_offset, &tmp,
		       sizeof(tmp));
		break;

	case R_KVX_64:
		tmp64 = (uintptr_t) mppa_dl_sym_lookup(hdl, &hdl->strtab[sym.st_name], 0);
		if (tmp64 == 0 && mppa_dl_errno_get_status() != E_NONE
		    && ELFKVX_ST_BIND(sym.st_info) != STB_WEAK)
			return -2;
		tmp64 += (ElfKVX_Lword)rel.r_addend;
		memcpy((void *)hdl->addr + rel.r_offset, &tmp64,
		       sizeof(tmp64));
		break;

	/* The following 3 relocations will act on 32bits or 64bits, depending on the target */
	case R_KVX_RELATIVE:
	        R_TMP = ((R_EWORD)rel.r_addend + (uintptr_t) hdl->addr);
		memcpy((void*)((uintptr_t)hdl->addr + rel.r_offset), &R_TMP,
			sizeof(R_EWORD));
		MPPA_DL_LOG(2, ">> Wrote 0x%lx at 0x%lx\n",
			    (long unsigned)R_TMP, (long unsigned) ((uintptr_t)hdl->addr + rel.r_offset));

		break;

	/* We are not implementing any lazy binding, so treat JMP_SLOT as GLOB_DAT. */
	case R_KVX_GLOB_DAT:
	case R_KVX_JMP_SLOT:
		R_TMP = (uintptr_t) mppa_dl_sym_lookup(hdl, &hdl->strtab[sym.st_name], 0);
		if (R_TMP == 0 && mppa_dl_errno_get_status() != E_NONE
		    && ELFKVX_ST_BIND(sym.st_info) != STB_WEAK)
			return -2;

		memcpy((void *)hdl->addr + rel.r_offset, &R_TMP,
			sizeof(R_EWORD));
		break;

	default:
		MPPA_DL_LOG(2, ">> Unhandled relocation type %ld\n", rel.r_info);
		return -1;
	}

	MPPA_DL_LOG(1, "< mppa_dl_apply_rela(%p, rel=0x%lx,0x%lx,0x%lx)\n",
		    hdl, (long)rel.r_info, (long)rel.r_offset, (long)rel.r_addend);

	return 0;
}
