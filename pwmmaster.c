/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include "serialargparse.h"
#include "serial.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define COMMAND_EXEC_BYTE 0xFD
#define COMMAND_RESET_BYTE 0xFF
#define ACK_BYTE 0xFF

extern uint16_t htons (uint16_t);
extern uint16_t ntohs (uint16_t);

struct Command {
  unsigned char pwm;
  uint16_t us;
} __attribute__((packed));

struct Command ntoh(const struct Command* command) {
  struct Command copy = {command->pwm, ntohs(command->us)};
  return copy;
}

struct Command hton(const struct Command* command) {
  struct Command copy = {command->pwm, htons(command->us)};
  return copy;
}

/**
 * @brief Send a reset signal to the connected device.  This is done
 * by sending COMMAND_RESET_BYTE for at least enough times for the
 * device to notice it as the first byte of a "command".
 * @param fd File descriptor where the reset will be sent.
 */
void reset(int fd) {
  unsigned char b = COMMAND_RESET_BYTE;
  for (int i = 0; i <= sizeof(struct Command); ++i) {
    writetty(fd, &b, 1);
  }
}

/**
 * @brief Send a command to the remote device connected to
 * the file descriptor.
 * @param fd Flle descriptor where command will be sent.
 * @param command The command to be written across the wire.
 */
void send_command(int fd, const struct Command* command) {
  struct Command data = hton(command);
  writetty(fd, &data, sizeof(struct Command));

  unsigned char b = readtty(fd);
  if (ACK_BYTE != b) {
    printf("expected %c, got %c", ACK_BYTE, b);
    abort();
  }
}

int main(int argc, char* argv[]) {
  const struct SerialOptions options = parse_opts(argc, argv);

  int fd = SerialOptions_setup(&options);
  reset(fd);

  while (1) {
    const unsigned char step = 4;
    uint16_t min = 1, max = 3000;
    for (uint16_t us = min; us <= max; us += step) {
      struct Command command = {'A', us};
      printf("sizeof(Command)=%ld", sizeof(struct Command));
      send_command(fd, &command);
      command.pwm = 'B';
      command.us = max-us;
      send_command(fd, &command);
    }

    for (uint16_t us = max; us >= min; us -= step) {
      struct Command command = {'A', us};
      send_command(fd, &command);
      command.pwm = 'B';
      command.us = max-us;
      send_command(fd, &command);
    }
  }

  return close(fd);
}
