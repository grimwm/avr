/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>
#include <assert.h>

#include <arpa/inet.h>

#include "joystick.h"
#include "io.h"
#include "serial.h"

#define CENTER_DEGREE 90

#ifndef DEFAULT_JOYSTICK_DEVICE
#  define DEFAULT_JOYSTICK_DEVICE "/dev/input/js0"
#endif

#define MIN(x,y) ((x) < (y) ? (x) : (y))

/* Options that may be set from the command-line. */
static char ttyDevicePath[PATH_MAX] = DEFAULT_TTY_DEVICE;
static char jsDevicePath[PATH_MAX] = DEFAULT_JOYSTICK_DEVICE;
static char jsOptionsPath[PATH_MAX] = "/etc/jsmaster.conf";
static SerialOptions serialOptions;

typedef struct {
  uint8_t msgid;        // msg correlation id
  uint8_t command;      // command for remote
  uint16_t value;       // command argument value
} __attribute__((packed)) Command;

/**
 * @brief Prepare a Command structure for wire transmission, by making sure
 * all the bytes are in network order.
 * @param msgid
 * @param command
 * @param value
 * @return A command in network byte order.
 */
Command Command_init(char msgid, char command, uint16_t value) {
  Command cmd = {msgid, command, htons(value)};
  return cmd;
}

void print_usage(const char *prog) {
  printf("Usage: %s [-Db25678e]\n", prog);
  puts("  -t --tty      device to use (default /dev/ttyAMA0)\n"
       "  -j --joystick device to use (default /dev/input/js0\n"
       "  -c --config   joystick mapping file (default /etc/jsmaster.conf)\n"
       "  -b --baud     baud rate (default 9600)\n"
       "  -2            use two stop bits instead of one\n"
       "  -5            bits per word\n"
       "  -6            bits per word\n"
       "  -7            bits per word\n"
       "  -8            bits per word (default)\n"
       "  -e            even parity (default odd)\n");
  exit(1);
}

void parse_opts(int argc, char *argv[]) {
  static const struct option lopts[] = {
    { "tty",      1, 0, 't' },
    { "joystick", 1, 0, 'j' },
    { "config",   1, 0, 'c' },
    { "baud",     1, 0, 'b' },
    { NULL,       0, 0, '2' },
    { NULL,       0, 0, '5' },
    { NULL,       0, 0, '6' },
    { NULL,       0, 0, '7' },
    { NULL,       0, 0, '8' },
    { NULL,       0, 0, 'e' },
    { NULL,       0, 0, 0 },
  };

  while (1) {
    int c = getopt_long(argc, argv, "t:j:c:b:25678e", lopts, NULL);
    if (-1 == c) {
      break;
    }
    
    switch (c) {
    case 't': {
      size_t len = MIN(strlen(optarg), PATH_MAX);
      if (!strncpy(ttyDevicePath, optarg, len)) {
        perror("Copying tty path.");
        abort();
      }
      ttyDevicePath[len] = '\0';
      break;
    }
    case 'j': {
      size_t len = MIN(strlen(optarg), PATH_MAX);
      if (!strncpy(jsDevicePath, optarg, len)) {
        perror("Copying joystick path.");
        abort();
      }
      jsDevicePath[len] = '\0';
      break;
    }
    case 'c': {
      size_t len = MIN(strlen(optarg), PATH_MAX);
      if (!strncpy(jsOptionsPath, optarg, len)) {
        perror("Copying joystick config path.");
        abort();
      }
      jsOptionsPath[len] = '\0';
      break;
    }
    case 'b': {
      long val = strtol(optarg, NULL, 10);
      if (LONG_MIN == val || LONG_MAX == val || val > (uint32_t)-1) {
        perror("Invalid baud rate given.");
        abort();
      }

      serialOptions.baudrate = val;
      break;
    }
    case '2':
      serialOptions.stop_bits = 2;
      break;
    case '5':
    case '6':
    case '7':
    case '8':
      serialOptions.bits_per_word = c-'0';
      break;
    case 'e':
      serialOptions.parity = 'e';
      break;
    default:
      print_usage(argv[0]);
      exit(1);
    }
  }
}

int main(int argc, char* argv[]) {
  SerialOptions_init(&serialOptions);
  parse_opts(argc, argv);

  JoystickOptions jsOpts = JoystickOptions_init(jsDevicePath, jsOptionsPath);
  printf("device: %s\ny_left:%d\ny_right:%d\n",
         jsOpts.devicepath, jsOpts.y_left, jsOpts.y_right);

  Joystick js = Joystick_open(&jsOpts);

  /* Check that axes are in valid ranges. */
  if (jsOpts.y_left < 0 || jsOpts.y_left >= js.naxes) {
    fprintf(stderr, "y_left=%d out of range=[0,%d] of axes", jsOpts.y_left, js.naxes);
  }
  if (jsOpts.y_right < 0 || jsOpts.y_right >= js.naxes) {
    fprintf(stderr, "y_right=%d out of range=[0,%d] of axes", jsOpts.y_right, js.naxes);
  }

  printf("Joystick driver version: %d.%d.%d\n",
         js.driverVersion >> 16, (js.driverVersion >> 8) & 0xff, js.driverVersion & 0xff);
  printf("Device name: %s\n", js.name);

  int serialfd = SerialOptions_open(&serialOptions);

  while (1) {
    JoystickEvent jsevent;

    while (1 == Joystick_getEvent(&js, &jsevent)) {
      switch (jsevent.type & ~JSE_INIT) {
      case JSE_AXIS: {
        if (jsOpts.y_left == jsevent.number || jsOpts.y_right == jsevent.number) {
          // TODO send the command aio_write
          static uint8_t msgid = 0xFF;
          Command cmd = Command_init(++msgid, jsOpts.y_left == jsevent.number ? 'L' : 'R',
                                     CENTER_DEGREE + (CENTER_DEGREE * -jsevent.value) / 0x7FFF);
          writetty(serialfd, &cmd, sizeof(Command));

          uint8_t ack = readtty(serialfd);
          printf("Command: %u, %u, %c, %d, %d\r", cmd.msgid, ack, cmd.command, ntohs(cmd.value), -jsevent.value);
        }

        break;
      }
      }
    }

    if (EAGAIN != errno) {
      pabort("Error getting js event");
    }
    
    fflush(stdout);
  }
}
