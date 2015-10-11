/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>

off_t filesize(int fd) {
  off_t curr = lseek(fd, 0, SEEK_CUR);
  assert(-1 != curr);

  off_t filelen = lseek(fd, 0, SEEK_END);
  assert(-1 != filelen);

  curr = lseek(fd, curr, SEEK_SET);
  assert(-1 != curr);

  return filelen;
}

void pabort(const char *fmt, ...) {
  int eno = errno;
  char buffer[PABORT_CHAR_BUFFER_LEN];
  va_list varargs;
  va_start(varargs, fmt);
  vsnprintf(buffer, PABORT_CHAR_BUFFER_LEN, fmt, varargs);
  va_end(varargs);
  errno = eno;
  perror(buffer);
  abort();
}
