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
#include <getopt.h>
#include <string.h>

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

struct SerialOptions parse_opts(int argc, char *argv[]) {
  struct SerialOptions options;
  SerialOptions_init(&options);
  
  while (1) {
    static const struct option lopts[] = {
      { "device",  1, 0, 'D' },
      { "baud",    1, 0, 'b' },
      { NULL,      0, 0, '5' },
      { NULL,      0, 0, '6' },
      { NULL,      0, 0, '7' },
      { NULL,      0, 0, '8' },
      { "even",    0, 0, 'e' },
      { NULL,      0, 0, '2' },
      { NULL, 0, 0, 0 },
    };

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
    case 'b':
    {
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
