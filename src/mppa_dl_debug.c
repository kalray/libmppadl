/**
 * Copyright (C) 2019 Kalray SA.
 *
 * All rights reserved.
 */

#include <libelf.h>
#include <string.h>
#include "mppa_dl_types.h"

#ifdef __CLUSTER_OS__
#include <hal/cos_bsp.h>
#define __MPPA_DL_DEBUG_SECTION_TEXT__ __COS_USER_SMEM_TEXT_SECTION__
#define __MPPA_DL_DEBUG_SECTION_DATA__ __COS_USER_SMEM_DATA_SECTION__
#else
#define __MPPA_DL_DEBUG_SECTION_TEXT__
#define __MPPA_DL_DEBUG_SECTION_DATA__
#endif

__MPPA_DL_DEBUG_SECTION_TEXT__
static void mppa_dl_debug_state_changed(void)
{
}

__MPPA_DL_DEBUG_SECTION_DATA__
struct mppa_dl_debug_s mppa_dl_debug = {
	.version = DL_VERSION,
	.head = 0,
	.brk = &mppa_dl_debug_state_changed,
	.valid = 1,
};

void mppa_dl_debug_set_valid(uint64_t valid)
{
	__builtin_kvx_fence();
	mppa_dl_debug.valid = valid;
	__builtin_kvx_fence();

	#ifdef __CLUSTER_OS__
	cos_duplicate_cluster_data(&mppa_dl_debug.valid,
		sizeof(mppa_dl_debug.valid));
	#endif
}

void mppa_dl_debug_update(struct mppa_dl_handle *phdl)
{
	struct mppa_dl_debug_map_s **p;

	mppa_dl_debug_set_valid(0);

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

	#ifdef __CLUSTER_OS__
	cos_duplicate_cluster_data(&mppa_dl_debug.head,
		sizeof(mppa_dl_debug.head));
	#endif

	mppa_dl_debug_set_valid(1);

	/* allow to see the changes */
	((void (*)(void)) mppa_dl_debug.brk)();
}

void mppa_dl_debug_init_debug(struct mppa_dl_handle *hdl)
{
	memset(&hdl->debug_map, 0, sizeof (hdl->debug_map));
}
