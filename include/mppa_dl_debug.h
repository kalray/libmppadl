/**
 * Copyright (C) 2019 Kalray SA.
 *
 * All rights reserved.
 */

#ifndef _MPPA_DL_DEBUG_H_
#define _MPPA_DL_DEBUG_H_

#include <stdint.h>

#define DL_VERSION 1

struct mppa_dl_handle;

/**
 * Structure for the debugger shared object map.
 * It describes a shared object for the debugger.
 */
struct mppa_dl_debug_map_s {
	uint64_t load_addr;               /* Load address for the dynamic object. */
	uint64_t name;                    /* File name of the dynamic object. */
	uint64_t name_len;                /* File name length of the dynamic object. */
	uint64_t next;                    /* Chain of loaded objects. */
} __attribute__((packed));

/**
 * Structure for debug
 * Used to communicate information to the debugger
 */
struct mppa_dl_debug_s {
	uint64_t version;                 /* Protocol version. */
	uint64_t head;                    /* Head of the shared object chain. */
	union {
		void *brk;                /* Address of the breakpoint function. */
		uint64_t brk_u64;
	};
} __attribute__((packed));

void mppa_dl_debug_init_debug(struct mppa_dl_handle *hdl);
void mppa_dl_debug_update(struct mppa_dl_handle *hdl);

#endif // _MPPA_DL_DEBUG_H_

