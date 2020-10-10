/**
 * Copyright (C) 2020 Kalray SA.
 */

#ifndef MPPA_DL_TRACE_H
#define MPPA_DL_TRACE_H

struct mppa_dl_handle;

/**
 * Structure for trace
 * Contains the info required to allocate tracepoint ids to the loaded libraries
 */
struct mppa_dl_trace_info {
	unsigned short first_tp_id; /* the first assigned tracepoint id */
	unsigned short last_tp_id;  /* the last assigned tracepoint id */
	struct mppa_dl_handle *next; /* the next elf in the library list
				      * sorted by tracepoints range ids
				      */
};

void mppa_dl_trace_load(struct mppa_dl_handle *hdl, int so_nb_tps);
void mppa_dl_trace_unload(struct mppa_dl_handle *hdl);
void mppa_dl_trace_init(struct mppa_dl_handle *hdl);

#endif
