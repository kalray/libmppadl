/**
 * Copyright (C) 2018 Kalray SA.
 *
 * All rights reserved.
 *
 *
 * \file mppa_dl_error.h
 * \brief Failure management.
 */

#ifndef MPPA_DL_ERROR_H
#define MPPA_DL_ERROR_H

#include <stdio.h>
#include <stdlib.h>

/**
 * Exit on failure.
 * Print the location of a failure on stderr, then exit.
 *
 * \param file The file where the failure occured.
 * \param line The line number near the failure location.
 */
void mppa_dl_failure(char * file, int line);

#endif
