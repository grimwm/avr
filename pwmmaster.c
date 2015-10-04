/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <termios.h>
#include <string.h> // memset
#include <errno.h>

#define COMMAND_EXEC_BYTE 0xFD
#define BAD_BYTE 0xFE
#define ACK_BYTE 0xFF

struct Command {
  unsigned char servo;
  uint16_t us;
} __attribute__((packed));

static void pabort(const char *s)
{
  perror(s);
  abort();
}

static const char* device = "/dev/ttyAMA0";

/**
 * Send data across the wire in the order it appears, one byte at a time.
 * File descriptor must be nonblocking.
 */
void send(int fd, unsigned char data) {
  while (write(fd, &data, 1) < 1) {
    if (EAGAIN != errno) {
      pabort("writing data");
    }
  }
}

unsigned char receive(int fd) {
  unsigned char b;
  
  while (read(fd, &b, 1) < 1) {
    if (EAGAIN != errno) {
      pabort("reading tty");
    }
  }

  return b;
}

void reset(int fd) {
  for (int i = 0; i < 6; ++i) {
    send(fd, 0xFF);
  }
}

void send_command(int fd, unsigned char servo, uint16_t us) {
  send(fd, servo);
  send(fd, (us >> 8) & 0xFF);
  send(fd, us & 0xFF);

  unsigned char b = receive(fd);
  if (ACK_BYTE != b) {
    printf("expected %c, got %c", ACK_BYTE, b);
    abort();
  /* } else { */
  /*   printf("ACK %c%04X, %c%dus\n", servo, us, servo, us); */
  }
}

int main(int argc, char* argv[]) {
  int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (-1 == fd) {
    pabort("can't open device");
  }

  if (!isatty(fd)) {
    pabort("isatty");
  }

  struct termios options;
  memset(&options, 0, sizeof(struct termios));

  options.c_oflag &= ~OPOST;

  /* Enable local line (no owner change) and enable receiver. */
  options.c_cflag |= CLOCAL | CREAD;
  /* Enable odd parity */
  options.c_cflag |= PARENB | PARODD;

  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  speed_t speed = B38400;
  if (-1 == cfsetispeed(&options, speed)) {
    pabort("Error setting input speed");
  }
  if (-1 == cfsetospeed(&options, speed)) {
    pabort("Error setting output speed");
  }

  /* Only send data after 1 chars are in the buffer.  Don't use a timer. */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_cc[VMIN] = 1;

  if (-1 == tcsetattr(fd, TCSAFLUSH, &options)) {
    pabort("Error setting serial line options");
  }

  reset(fd);

  while (1) {
    const uint16_t min = 1, max = 3000;
    const unsigned char step = 4;
    for (uint16_t us = min; us <= max; us += step) {
      send_command(fd, 'A', us);
      send_command(fd, 'B', max-us);
    }

    for (uint16_t us = max; us >= min; us -= step) {
      send_command(fd, 'A', us);
      send_command(fd, 'B', max-us);
    }
  }

  return close(fd);
}
