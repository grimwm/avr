/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include "serial.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

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

void print_usage(const char *prog) {
  printf("Usage: %s [-Db25678e]\n", prog);
  puts("  -D --device   device to use (default /dev/ttyAMA0)\n"
       "  -b --baud     baud rate (default 9600)\n"
       "  -2            use two stop bits instead of one\n"
       "  -5            bits per word\n"
       "  -6            bits per word\n"
       "  -7            bits per word\n"
       "  -8            bits per word (default)\n"
       "  -e            even parity (default odd)\n");
  exit(1);
}

SerialOptions parse_opts(int argc, char *argv[]) {
  SerialOptions options;
  SerialOptions_init(&options);
  
  static const struct option lopts[] = {
    { "device",  1, 0, 'D' },
    { "baud",    1, 0, 'b' },
    { NULL,      0, 0, '2' },
    { NULL,      0, 0, '5' },
    { NULL,      0, 0, '6' },
    { NULL,      0, 0, '7' },
    { NULL,      0, 0, '8' },
    { NULL,      0, 0, 'e' },
    { NULL, 0, 0, 0 },
  };

  while (1) {
    int c = getopt_long(argc, argv, "D:b:25678e", lopts, NULL);
    if (-1 == c) {
      break;
    }
    
    switch (c) {
    case 'D':
      if (!strncpy(options.device, optarg, PATH_MAX < strlen(optarg) ? PATH_MAX : strlen(optarg))) {
        perror("Copying device to options.");
        abort();
      }
      break;
    case 'b': {
      long val = strtol(optarg, NULL, 10);
      if (LONG_MIN == val || LONG_MAX == val || val > (uint32_t)-1) {
        perror("Invalid baud rate given.");
        abort();
      }
      
      options.baudrate = val;
      break;
    }
    case '2':
      options.stop_bits = 2;
      break;
    case '5':
    case '6':
    case '7':
    case '8':
      options.bits_per_word = c-'0';
      break;
    case 'e':
      options.parity = 'e';
      break;
    default:
      print_usage(argv[0]);
      break;
    }
  }

  return options;
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
  const SerialOptions options = parse_opts(argc, argv);

  int fd = SerialOptions_open(&options);
  reset(fd);

  while (1) {
    const unsigned char step = 4;
    const uint16_t min = 1, max = 3000;
    for (uint16_t us = min; us <= max; us += step) {
      struct Command command = {'A', us};
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
