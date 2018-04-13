/**
 * Copyright (C) 2018 Kalray SA.
 */

/**
 * Current autotraces support is inherited from the previous mppa_dl library.
 * This feature is required by autotraces testsuite, even if such tests are now
 * disabled (autotraces does no longer support dynamic tracing). Consequently,
 * the following autotraces support is only added for compatibilty purposes and
 * can't be tested.
 */

#ifndef MPPADL_ENABLE_AUTOTRACE
#error This file should not be included when autotrace support is disabled
#endif

#ifndef MPPA_DL_AUTOTRACES_H
#define MPPA_DL_AUTOTRACES_H

#include "mppa_dl_error.h"
#include "mppa_dl_sym.h"

#define MPPA_TRACEPOINT_DEFINE

#undef MPPA_TRACEPOINT_PROVIDER
#define MPPA_TRACEPOINT_PROVIDER autotraces
#undef MPPA_TRACEPOINT_FILE
#define MPPA_TRACEPOINT_FILE mppa_dl_autotraces.h

#if !defined(_TRACE_H_) || defined(MPPA_TRACEPOINT_HEADER_MULTI_READ)
#define _TRACE_H_

#define MPPA_TRACEPOINT_DEC_FIELD_(x, y) MPPA_TRACEPOINT_DEC_FIELD(x, y)

#include "mppa_trace.h"

MPPA_DECLARE_TRACEPOINT(autotraces, loadTrace, (
				MPPA_TRACEPOINT_STRING_FIELD(char*, status),
				MPPA_TRACEPOINT_DEC_FIELD(int, dynamic_id),
				MPPA_TRACEPOINT_HEX_FIELD(void*, load_address)
				))

#endif

#define MPPA_CREATE_TRACEPOINT
#include "mppa_trace.h"

void mppa_dl_set_autotraces();
void mppa_dl_clear_autotraces();
int mppa_dl_get_dynamic_id(void *);

#endif
