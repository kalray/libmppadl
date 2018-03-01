/**
 * Copyright (C) 2018 Kalray SA.
 */

#include "mppa_dl_autotraces.h"

/**
 * This function is an mutator for enable_autoraces,
 * which enable autotraces for dynamic module
 */
void mppa_dl_set_autotraces()
{
	mppa_dl_autotraces = 1;
}

/**
 * This function is a mutator for enable_autoraces,
 * which disable autotraces for dynamic module
 */
void mppa_dl_clear_autotraces()
{
	mppa_dl_autotraces = 0;
}

int mppa_dl_get_dynamic_id(void *handle)
{
	void *dyn_id;

	dyn_id = mppa_dl_sym_lookup((mppa_dl_handle_t *)handle,
				    "__mppa_autotraces_id", 1);

	if (dyn_id != NULL ||
	    (dyn_id == NULL && mppa_dl_errno_get_status() == E_NONE))
		return (int) dyn_id;

	return -1;
}
