/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 */

#include "mppa_dl_error.h"

int __mppa_dl_loglevel = 0;

void mppa_dl_errno(int errno)
{
	mppa_dl_errno_status = errno;
}

char *mppa_dl_error(void)
{
	char *err_msg;

	switch (mppa_dl_errno_status) {
	case E_NONE:
		err_msg = NULL;
		break;
	case E_ELF_OLD:
		err_msg = "libelf too old";
		break;
	case E_ELF_MEM:
		err_msg = "libelf failure: elf_memory()";
		break;
	case E_ELF_PHDRNUM:
		err_msg = "libelf failure: elf_getphdrnum()";
		break;
	case E_ELF_PHDR:
		err_msg = "libelf failure: elf_getphdr()";
		break;
	case E_ELF_SHDR:
		err_msg = "libelf failure: elf_getshdr()";
		break;
	case E_ELF_END:
		err_msg = "libelf failure: elf_end() doesn't return 0";
		break;
	case E_ELF_SYM:
		err_msg = "libelf failure: gelf_getsym()";
		break;
	case E_MEM_ALIGN:
		err_msg = "memalign() failed";
		break;
	case E_UNKN_RELOC:
		err_msg = "relocation not supported";
		break;
	case E_HDL_LIST:
		err_msg = "unload failure: cannot find handle in list";
		break;
	default:
		err_msg = "unknown error";
	}

	mppa_dl_errno_status = E_NONE;

	return err_msg;
}
