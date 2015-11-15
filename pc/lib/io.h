/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#pragma once

#include <sys/types.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculates the size of a file.  Note that this function is not thread-safe.
 * @param fd File descriptor.
 * @return The size of the file given by fd.
 */
off_t filesize(int fd);

#define PABORT_CHAR_BUFFER_LEN 1024

/**
 * @brief Formats a string, calls perror(3) with it, and then calls abort(3).
 * @param fmt The format string.  You must also pass its format arguments after it.
 */
void pabort(const char* fmt, ...);

#ifdef __cplusplus
} // extern "C"
#endif
