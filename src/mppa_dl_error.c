/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_error.h"

int mppa_dl_loglevel = 0;
int mppa_dl_errno_status = E_NONE;

int mppa_dl_autotraces = 0;

void mppa_dl_set_loglevel(int level)
{
	mppa_dl_loglevel = level;
}


void mppa_dl_errno(int errno)
{
	mppa_dl_errno_status = errno;
}


int mppa_dl_errno_get_status()
{
	return mppa_dl_errno_status;
}


char *mppa_dl_error(void)
{
	char *err_msg;

	switch (mppa_dl_errno_status) {
	case E_NONE:
		err_msg = NULL;
		break;
	case E_MEM_ALIGN:
		err_msg = "memalign() failed";
		break;
	case E_RELOC:
		err_msg = "relocation failed";
		break;
	case E_PLT_RELOC:
		err_msg = "relocation of type DT_REL not supported";
		break;
	case E_NO_SYM:
		err_msg = "no symbol found";
		break;
	case E_SYM_OUT:
		err_msg = "symbol index out of range";
		break;
	case E_END_CHAIN:
		err_msg = "symbol not found, chain ends up with zero";
		break;
	case E_INIT_HDL:
		err_msg = "handle initialization failed";
		break;
	default:
		err_msg = "unknown error";
	}

	mppa_dl_errno_status = E_NONE;

	return err_msg;
}
