/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include "serial.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h> // memset
#include <errno.h>

static void pabort(const char *s) {
  perror(s);
  abort();
}

void SerialOptions_init(SerialOptions* opts) {
  strncpy(opts->device, DEFAULT_TTY_DEVICE, PATH_MAX);
  opts->bits_per_word = DEFAULT_TTY_BPW;
  opts->baudrate = DEFAULT_TTY_BAUD;
  opts->parity = DEFAULT_TTY_PARITY;
  opts->stop_bits = DEFAULT_TTY_STOP_BITS;
}

int SerialOptions_open(const SerialOptions* opts) {
  /* int fd = open(opts->device, O_RDWR | O_NOCTTY | O_NONBLOCK); */
  int fd = open(opts->device, O_RDWR | O_NOCTTY);
  if (-1 == fd) {
    pabort("can't open device");
  }

  if (!isatty(fd)) {
    pabort("isatty");
  }

  struct termios termopts;
  memset(&termopts, 0, sizeof(struct termios));

  /* Enable local line (no owner change) and enable receiver. */
  termopts.c_cflag |= CLOCAL | CREAD;
  /* Stop bits default to 1, but set it to 2 if necessary. */
  if (2 == opts->stop_bits) {
    termopts.c_cflag |= CSTOPB;
  }
  /* Enable parity.  It defaults to even parity. */
  termopts.c_cflag |= PARENB;
  if ('o' == opts->parity) {
    /* Change parity to odd. */
    termopts.c_cflag |= PARODD;
  }

  /* Set the number of bits in the data portion of a packet. */
  termopts.c_cflag &= ~CSIZE;
  switch (opts->bits_per_word) {
  case 5:
    termopts.c_cflag |= CS5;
    break;
  case 6:
    termopts.c_cflag |= CS6;
    break;
  case 7:
    termopts.c_cflag |= CS7;
    break;
  case 8:
    termopts.c_cflag |= CS8;
    break;
  }

  speed_t speed = 0;
  switch (opts->baudrate) {
  case 2400:
    speed = B2400;
    break;
  case 9600:
    speed = B9600;
    break;
  case 19200:
    speed = B19200;
    break;
  case 38400:
    speed = B38400;
    break;
  case 57600:
    speed = B57600;
    break;
  case 115200:
    speed = B115200;
    break;
  case 230400:
    speed = B230400;
    break;
  default:
    fprintf(stderr, "Invalid baud rate: %d", opts->baudrate);
    abort();
  }
  if (-1 == cfsetispeed(&termopts, speed)) {
    pabort("Error setting input speed");
  }
  if (-1 == cfsetospeed(&termopts, speed)) {
    pabort("Error setting output speed");
  }

  termopts.c_oflag &= ~OPOST;

  /*
   * Only send data after 1 chars are in the buffer.
   * Don't use a timer.
   */
  termopts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG | IEXTEN);
  termopts.c_cc[VMIN] = 1;

  if (-1 == tcsetattr(fd, TCSAFLUSH, &termopts)) {
    pabort("Error setting serial line options");
  }

  return fd;
}

void writetty(int fd, const void* data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    if (-1 == write(fd, data+i, 1)) {
      pabort("writing data to tty");
    }
    if (-1 == tcdrain(fd)) {
      pabort("draining tty");
    }
  }
  /* if (-1 == write(fd, data, length)) { */
  /*   pabort("wrtiing data to tty"); */
  /* } */
}

uint8_t readtty(int fd) {
  uint8_t b;

  if (-1 == read(fd, &b, sizeof(uint8_t))) {
    pabort("reading tty");
  }

  return b;
}
