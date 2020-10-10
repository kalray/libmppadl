/**
 * Copyright (C) 2020 Kalray SA.
 */

#include <string.h>
#include "mppa_dl.h"

#define MPPA_TRACE_ENABLE
#include "mppa_trace.h"

/* tracepoints emitted when a shared library is loaded/unloaded */
MPPA_DECLARE_TRACEPOINT(mppadl, load, (
	MPPA_TRACEPOINT_STRING_FIELD(char *, so_name),
	MPPA_TRACEPOINT_DEC_FIELD(unsigned short, first_tp_id),
	MPPA_TRACEPOINT_DEC_FIELD(unsigned short, last_tp_id),
	MPPA_TRACEPOINT_HEX_FIELD(void*, load_address)
	))

MPPA_DECLARE_TRACEPOINT(mppadl, unload, (
	MPPA_TRACEPOINT_STRING_FIELD(char *, so_name),
	MPPA_TRACEPOINT_HEX_FIELD(void*, load_address)
	))

/* tracepoints IDs are unsigned short values (ID 0 is not used) */
#define MPPA_DL_MAX_TP_ID ((1UL << (sizeof(unsigned short) * 8)) - 1)

/* declaration of the variables for the beginning and the end of the
 * kvx_tp_id section of the executable
 */
extern void *__stop_kvx_tp_id, *__start_kvx_tp_id;

/* the head of libraries linked list sorted by the tracepoints ID ranges */
static mppa_dl_handle_t *head_tp;

static int mppa_dl_check_tp_id_range_overflow(const char *func_name,
	struct mppa_dl_handle *hdl, unsigned short last_tp_id, int so_nb_tps)
{
	/* verify that the tracepoint ID range does not overflow the
	 * the tracepoint ID data type (unsigned short)
	 */
	if (((unsigned long) last_tp_id) + so_nb_tps > MPPA_DL_MAX_TP_ID) {
		MPPA_DL_LOG(1, "> %s(%p): not enough tracepoints available "
			"(first ID available: %lu, IDs required number: %lu)\n",
			func_name, hdl, (unsigned long) last_tp_id + 1,
			(unsigned long) so_nb_tps);
		return 1;
	}

	return 0;
}

void mppa_dl_trace_init(struct mppa_dl_handle *hdl)
{
	memset(&hdl->trace_info, 0, sizeof(hdl->trace_info));
}

void mppa_dl_trace_load(struct mppa_dl_handle *hdl, int so_nb_tps)
{
	struct mppa_dl_handle *q, *p;
	unsigned short *p_first_id, exe_nb_tps, first_id;
	const char *name = hdl->name ? hdl->name : "?";
	uintptr_t long_exe_nb_tps;

	MPPA_DL_LOG(1, "< %s(%p, %d)\n", __func__, hdl, so_nb_tps);

	if (!so_nb_tps) {
		MPPA_DL_LOG(1, "> %s(%p): no tracepoint\n", __func__, hdl);
		return;
	}

	/* get the address of the variable which specifies the first tracepoint
	 * ID in the new loaded shared library
	 */
	p_first_id = (unsigned short *) mppa_dl_sym_lookup_local(hdl,
		"__kvx_tracepoints_first_id");
	if (!p_first_id) {
		MPPA_DL_LOG(1, "> %s(%p): no __kvx_tracepoints_first_id sym\n",
			    __func__, hdl);
		return;
	}

	/* get the number of tracepoints in the executable. Section kvx_tp_id
	 * contains 1 byte for each tracepoint
	 */
	long_exe_nb_tps = (uintptr_t) &__stop_kvx_tp_id -
		(uintptr_t) &__start_kvx_tp_id;
	if (long_exe_nb_tps > MPPA_DL_MAX_TP_ID) {
		MPPA_DL_LOG(1, "> %s(%p): the executable has too many "
			    "tracepoints (%lu)\n",
			    __func__, hdl, long_exe_nb_tps);
		return;
	}
	exe_nb_tps = (unsigned short) long_exe_nb_tps;

	/* using the trace_info.next field, the new loaded shared libraries are
	 * sorted by the tracepoints range ID allocated to them.
	 * If there is no library in the sorted list or if between the
	 * tracepoints ID range of the executable and the range of the first
	 * library in the list there is enough space for the new added library
	 * ID range, place the new added library at the beginning of the list
	 */
	if (head_tp == NULL ||
	    head_tp->trace_info.first_tp_id - exe_nb_tps > so_nb_tps) {
		if (mppa_dl_check_tp_id_range_overflow(__func__, hdl,
		    exe_nb_tps, so_nb_tps))
			return;
		first_id = exe_nb_tps + 1;
		hdl->trace_info.next = head_tp;
		head_tp = hdl;
	} else {
		/* search in the sorted list to see if between 2 consecutive
		 * libraries (p and q) there is enough space for the loaded
		 * library tracepoints ID range
		 */
		p = head_tp;
		for (q = p->trace_info.next; q; p = q, q = q->trace_info.next)
			if (q->trace_info.first_tp_id - p->trace_info.last_tp_id
			    > so_nb_tps) {
				first_id = p->trace_info.last_tp_id + 1;
				p->trace_info.next = hdl;
				hdl->trace_info.next = q;
				break;
			}

		/* if no place was found between 2 shared libraries, place the
		 * new loaded library at the end of the sorted list
		 */
		if (!q) {
			if (mppa_dl_check_tp_id_range_overflow(__func__, hdl,
			    p->trace_info.last_tp_id, so_nb_tps))
				return;
			first_id = p->trace_info.last_tp_id + 1;
			p->trace_info.next = hdl;
		}
	}

	/* inform the new loaded library about its first tracepoint ID */
	*p_first_id = first_id;

	/* set the remaining fields of the trace_info struct */
	hdl->trace_info.first_tp_id = first_id;
	hdl->trace_info.last_tp_id = first_id + so_nb_tps - 1;

	/* generate a tracepoint to inform the trace data parser about the new
	 * loaded library
	 */
	mppa_tracepoint(mppadl, load, name, hdl->trace_info.first_tp_id,
		hdl->trace_info.last_tp_id, hdl->addr);

	MPPA_DL_LOG(1, "< %s(%p)\n", __func__, hdl);
}

void mppa_dl_trace_unload(struct mppa_dl_handle *hdl)
{
	struct mppa_dl_handle **p;
	const char *name = hdl->name ? hdl->name : "?";

	MPPA_DL_LOG(1, "< %s(%p)\n", __func__, hdl);

	if (!hdl->trace_info.first_tp_id) {
		MPPA_DL_LOG(1, "> %s(%p): no tracepoint\n", __func__, hdl);
		return;
	}

	/* remove the unloaded library from the sorted list */
	for (p = &head_tp; *p; p = &(*p)->trace_info.next)
		if (*p == hdl) {
			/* inform the trace data parser about the unloaded
			 * library
			 */
			mppa_tracepoint(mppadl, unload, name, hdl->addr);

			*p = (*p)->trace_info.next;
			break;
		}

	MPPA_DL_LOG(1, "> %s(%p)\n", __func__, hdl);
}
