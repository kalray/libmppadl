/**
 * Copyright (C) 2018 Kalray SA.
 *
 *
 * \file mppa_dl.h
 * \brief libmppadl public interface.
 */

#ifndef MPPA_DL_H
#define MPPA_DL_H

#include <string.h>
#include <libelf.h>

#include "mppa_dl_types.h"
#include "mppa_dl_error.h"
#include "mppa_dl_mem.h"
#include "mppa_dl_load.h"
#include "mppa_dl_sym.h"


/**
 * \brief Load a dynamic library.
 *
 * The function mppa_dl_load() loads a dynamic library memory image, pointed by
 * \a image and returns an opaque "handle" for the dynamic library.
 *
 * \param image Pointer to the start of the memory image to load.
 * \param flag Specify if symbols defined by this image will be made available
 * (MPPA_DL_GLOBAL), or won't be made available (MPPA_DL_LOCAL), for symbol
 * resolution of subsequently loaded images.
 * \return The \a handle of the loaded image on success, or NULL in
 * case of error (see mppa_dl_error())
 */
void *mppa_dl_load(const char *image, int flag);

/**
 * \brief Get the memory address of a symbol.
 *
 * The function mppa_dl_sym() returns the memory address of the null-terminated
 * symbol \a symbol from the \a handle of the dynamic library returned by
 * mppa_dl_load().
 *
 * \param handle The \a handle of the dynamic library where to look for
 * \a symbol.
 * \param symbol The symbol name (a null-terminated string).
 * \return The memory location where the symbol is loaded on success, or NULL on
 * error (see mppa_dl_error())
 */
void *mppa_dl_sym(void *handle, const char* symbol);

/**
 * \brief Unload a dynamic library.
 *
 * The mppa_dl_unload() unload the dynamic library handled by \a handle.
 *
 * \param handle The \a handle of the dynamic library to unload.
 * \return Zero on success, and nonzero on error (see mppa_dl_error()).
 */
int mppa_dl_unload(void *handle);

/**
 * \brief Informs the trace system that a library was loaded
 *
 * The function mppa_dl_load() pushes a tracepoint which informs the trace
 * system about the library load address, name and tracepoints IDs range.
 * mppa_dl_trace_load_secondary() is called for the other clusters of the SMP
 * application. It pushes the same tracepoint so that the trace system has the
 * loaded library info for these clusters, too.
 *
 * \param handle The handle of the loaded dynamic library.
 * \param cluster_id Cluster ID for which the function is called
 */
void mppa_dl_trace_load_secondary(void *handle, int cluster_id);

/**
 * \brief Informs the trace system that a library was loaded
 *
 * The function mppa_dl_unload() pushes a tracepoint which informs the trace
 * system about the unloaded library address and its name.
 * mppa_dl_trace_unload_secondary() is called for the other clusters of the SMP
 * application.
 *
 * \param handle The handle of the loaded dynamic library.
 * \param cluster_id Cluster ID for which the function is called
 */
void mppa_dl_trace_unload_secondary(void *handle, int cluster_id);

/**
 * \brief Set the verbosity level of the library. Three log levels are supported.
 *   - level 0: logs are disabled.
 *   - level 1: enable minimal log level, i.e., control flow.
 *   - level 2: level 1 + more details about relocations.
 * The default log level is 0.
 *
 * \param level Debug verbosity level of the library.
 */
extern void mppa_dl_set_loglevel(int level);

/**
 * \brief Translate errors to human readable strings.
 *
 * The function mppa_dl_error() returns a human readable string describing the
 * newest error arisen in mppa_dl_load(), mppa_dl_sym() or mppa_dl_unload()
 * since its last call.
 *
 * \return A string only if an error has occurred since initialization or the
 * last call of mppa_dl_error(), and NULL otherwise
 */
extern char *mppa_dl_error(void);

/**
 * \brief Overload memalign, malloc, or free functions used by the
 * library.
 */
extern void mppa_dl_set_memalign(mppa_dl_memalign_t);
extern void mppa_dl_set_malloc(mppa_dl_malloc_t);
extern void mppa_dl_set_free(mppa_dl_free_t);

#endif
