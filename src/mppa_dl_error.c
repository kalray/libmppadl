/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_error.h"

int mppa_dl_loglevel = 0;
int mppa_dl_errno_status = E_NONE;

void mppa_dl_set_loglevel(int level)
{
	MPPA_DL_LOG(2, "- set log level to %d\n", level);

	mppa_dl_loglevel = level;
}


int mppa_dl_get_loglevel(void)
{
	return mppa_dl_loglevel;
}


void mppa_dl_errno(int errno)
{
	mppa_dl_errno_status = errno;
}


int mppa_dl_errno_get_status(void)
{
	return mppa_dl_errno_status;
}


char *mppa_dl_error(void)
{
	MPPA_DL_LOG(1, "> mppa_dl_error()\n");

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
	case E_WRONG_FLAG:
		err_msg = "wrong flag for mppa_dl_load()";
		break;
	case E_UNSUP_TLS:
		err_msg = "TLS data not supported";
		break;
	case E_UNSUP_CTOR_DTOR:
		err_msg = "Constructors/Destructors not supported";
		break;
	default:
		err_msg = "unknown error";
	}

	mppa_dl_errno_status = E_NONE;

	MPPA_DL_LOG(1, "< mppa_dl_error()\n");

	return err_msg;
}
