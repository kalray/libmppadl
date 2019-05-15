/**
 * Copyright (C) 2019 Kalray SA.
 *
 * All rights reserved.
 */

#include <libelf.h>
#include <string.h>
#include "mppa_dl_types.h"

static void mppa_dl_debug_state_changed(void)
{
}

struct mppa_dl_debug_s mppa_dl_debug = {
	.version = DL_VERSION,
	.head = 0,
	.brk = &mppa_dl_debug_state_changed,
};

void mppa_dl_debug_update(struct mppa_dl_handle *phdl)
{
	struct mppa_dl_debug_map_s **p;

	if (!phdl) {
		mppa_dl_debug.head = 0;
	} else {
		/* find the first element in the list (the executable
		   if it contains the dynamic section, else the first so */
		while (phdl->parent)
			phdl = phdl->parent;

		/* goto the first so */
		if (_DYNAMIC != 0)
			phdl = phdl->child;

		/* relink the debug_map of the sos and init the other fields */
		p = (struct mppa_dl_debug_map_s **) &mppa_dl_debug.head;
		while (phdl) {
			mppa_dl_debug_init_debug (phdl);

			phdl->debug_map.load_addr =
				(uint64_t) (uintptr_t) phdl->addr;
			phdl->debug_map.name =
				(uint64_t) (uintptr_t) phdl->name;
			if (phdl->name)
				phdl->debug_map.name_len =
					strlen(phdl->name) + 1;
			else
				phdl->debug_map.name_len = 0;

			*p = &phdl->debug_map;
			p = (struct mppa_dl_debug_map_s **) &(*p)->next;
			phdl = phdl->child;
		}

		/* set the next field of the last so debug_map */
		*p = NULL;
	}

	__builtin_k1_fence();

	/* allow to see the changes */
	((void (*)(void)) mppa_dl_debug.brk)();
}

void mppa_dl_debug_init_debug(struct mppa_dl_handle *hdl)
{
	memset(&hdl->debug_map, 0, sizeof (hdl->debug_map));
}
