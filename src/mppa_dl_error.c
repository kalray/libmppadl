/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_error.h"

void mppa_dl_errno(int errno)
{
	mppa_dl_errno_status = errno;
}

char *mppa_dl_error(void)
{
	char *err_msg;

	switch (mppa_dl_errno_status) {
	case E_NONE:
		err_msg = "no error";
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
	default:
		err_msg = "unknown error";
	}

	mppa_dl_errno_status = E_NONE;

	return err_msg;
}
