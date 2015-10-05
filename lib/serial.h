/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#pragma once

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFAULT_TTY_DEVICE
#  define DEFAULT_TTY_DEVICE "/dev/ttyAMA0"
#endif

#ifndef DEFAULT_TTY_BPW
#  define DEFAULT_TTY_BPW 8
#endif

#ifndef DEFAULT_TTY_BAUD
#  define DEFAULT_TTY_BAUD 9600
#endif

#ifndef DEFAULT_TTY_PARITY
#  define DEFAULT_TTY_PARITY 'o'
#endif

#ifndef DEFAULT_TTY_STOP_BITS
#  define DEFAULT_TTY_STOP_BITS 1
#endif

typedef struct {
  char device[PATH_MAX]; // path to serial device
  uint8_t bits_per_word;
  uint32_t baudrate;
  uint8_t parity;
  uint8_t stop_bits;
} SerialOptions;

/**
 * @brief Configure SerialOptions with some same defaults.
 * @param opts The options to be initialized.
 */
void SerialOptions_init(SerialOptions* opts);

/**
 * @brief Use "opts" to setup the serial device.
 * @param opts
 * @return The file descriptor to the serial device, configured
 *         according to "opts".  This may be closed using close(2).
 */
int SerialOptions_setup(const SerialOptions* opts);

/**
 * @brief Send a byte of data.  This function can handle a blocking
 * or non-blocking file descriptor.
 * @param fd The file descriptor where a byte will be written.
 * @param data The byte of data to write.
 */
void writetty(int fd, const void* data, size_t length);

/**
 * @brief Receive a byte of data.  The function can handle a
 * blocking or non-blocking file descriptor.
 * @param fd The file descriptor from which a byte will be read.
 * @returns A byte of data read from the file descriptor.
 */
unsigned char readtty(int fd);

#ifdef __cplusplus
} // extern "C"
#endif
