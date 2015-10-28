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

#include "io.h"
#include "serial.h"
#include "math.h"

#define SSIZET_FMT "%zd"

/* Options that may be set from the command-line. */
static char ttyDevicePath[PATH_MAX] = DEFAULT_TTY_DEVICE;
static char ihexFilePath[PATH_MAX];
static SerialOptions serialOptions;
static int verbose;

typedef struct {
  uint8_t  length;
  uint16_t address;
  uint8_t  type;
  uint8_t  data[0xFF];
  uint8_t  crc;
} IntelHexRecord;

void print_usage(const char *prog) {
  printf("Usage: %s [-tfb]\n", prog);
  puts("  -t --tty      device to use (default /dev/ttyAMA0)\n"
       "  -f --file     file containing ihex binary\n"
       "  -b --baud     baud rate (default 9600)\n"
       "  -v --verbose  Enable verbose output\n");
  exit(1);
}

void parse_opts(int argc, char *argv[]) {
  static const struct option lopts[] = {
    { "tty",      1, 0, 't' },
    { "file",     1, 0, 'f' },
    { "baud",     1, 0, 'b' },
    { "verbose",  0, 0, 'v' },
    { NULL,       0, 0, 0 },
  };

  while (1) {
    int c = getopt_long(argc, argv, "t:f:b:v", lopts, NULL);
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
    case 'f': {
      size_t len = MIN(strlen(optarg), PATH_MAX);
      if (!strncpy(ihexFilePath, optarg, len)) {
        perror("Copying .hex file path.");
        abort();
      }
      ihexFilePath[len] = '\0';
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
    case 'v':
      verbose = 1;
      break;
    default:
      print_usage(argv[0]);
      exit(1);
    }
  }

  if (strlen(ihexFilePath) == 0) {
    print_usage(argv[0]);
    exit(1);
  }
}

/**
 * @brief Convert a character to its hex equivalent.
 * @param hexch A character representing a hex digit.
 * @return The integer value of a hex character.
 */
static uint8_t charNibbleToUInt(char hexch) {
  hexch -= '0';
  if (hexch > 9) {
    hexch -= 7;         // 7 chars between '9' and 'A' on ascii table
  }

  assert(hexch >= 0 && hexch <= 15);
  return hexch;
}

/**
 * @brief Converts a character byte pair to its unsigned int equivalent.
 * @param hex Two hex characters.
 * @return The unsigned value of the input hex character pair.
 */
static uint8_t charByteToUInt(char hex[2]) {
  return charNibbleToUInt(hex[0]) << 4 | charNibbleToUInt(hex[1]);
}

int fetch_record(int fd, IntelHexRecord* ihex) {
  uint8_t crc;
  if (-1 == read(fd, &crc, 1)) {
    pabort("read start of ihex record");
  }
  assert(':' == crc);

  crc = 0;
  
  char byte[2];
  if (-1 == read(fd, byte, 2)) {
    pabort("read ihex length");
  }
  ihex->length = charByteToUInt(byte);
  crc += ihex->length;

  if (-1 == read(fd, byte, 2)) {
    pabort("read ihex address msb");
  }
  ihex->address = charByteToUInt(byte) << 8;
  crc += ihex->address >> 8;

  if (-1 == read(fd, byte, 2)) {
    pabort("read ihex address lsb");
  }
  ihex->address |= charByteToUInt(byte);
  crc += ihex->address & 0xFF;

  if (-1 == read(fd, byte, 2)) {
    pabort("read ihex type");
  }
  ihex->type = charByteToUInt(byte);
  crc += ihex->type;
    
  for (size_t i = 0; i < ihex->length; ++i) {
    if (-1 == read(fd, byte, 2)) {
      pabort("read ihex data");
    }

    ihex->data[i] = charByteToUInt(byte);
    crc += ihex->data[i];
  }

  if (-1 == read(fd, byte, 2)) {
    pabort("read ihex crc");
  }
  crc = ~crc + 1;
  if (crc != charByteToUInt(byte)) {
    fprintf(stderr, "expected crc %02x, got %02x", charByteToUInt(byte), crc);
    abort();
  }
  ihex->crc = crc;

  if (-1 == read(fd, byte, 2)) {
    pabort("chomp CR NL");
  }
  assert('\r' == byte[0]);
  assert('\n' == byte[1]);

  return 0;     // read one line of ihex
}

static void IntelHexRecord_print(FILE* s, const IntelHexRecord* record) {
  assert(s);
  fprintf(s,
          "record = {\n"
          "  length=%02x\n"
          "  address=%04x\n"
          "  type=%02x\n"
          "  data=",
          record->length, record->address, record->type);

  for (size_t i = 0; i < record->length; ++i) {
    fprintf(s, "%02x", record->data[i]);
  }
  fprintf(s,
          "\n  crc=%02x\n"
          "}\n",
          record->crc);
}

int main(int argc, char* argv[]) {
  SerialOptions_init(&serialOptions);
  parse_opts(argc, argv);

  int serialfd = SerialOptions_open(&serialOptions);

  int fd = open(ihexFilePath, O_RDONLY);
  if (-1 == fd) {
    pabort("open %s", ihexFilePath);
  }

  IntelHexRecord record;
  for (fetch_record(fd, &record); 1 != record.type; fetch_record(fd, &record)) {
    if (verbose) {
      static size_t lineno = 0;
      printf("Sending record #" SSIZET_FMT "\n", ++lineno);
      IntelHexRecord_print(stdout, &record);
    }

    /* Send the length of our data. */
    writetty(serialfd, &"L", 1);
    writetty(serialfd, &record.length, sizeof(uint8_t));
    uint8_t len = readtty(serialfd);
    if (len != record.length) {
      fprintf(stderr, "bad length response: expected %02x, got %02x\n", record.length, len);
      exit(1);
    }
    printf("%02x", len);

    /* Send the address for our data. */
    writetty(serialfd, &"A", 1);
    record.address = htons(record.address);
    writetty(serialfd, &record.address, sizeof(uint16_t));
    record.address = ntohs(record.address);

    uint8_t addrsum = readtty(serialfd);
    if (addrsum != (record.address >> 8) + (record.address & 0xFF)) {
      fprintf(stderr, "bad address sum: expected %02x, got %02x\n", (record.address >> 8) + (record.address & 0xFF), addrsum);
      exit(1);
    }
    printf("%04x", record.address);

    printf("%02x", record.type);

    /* Send our binary data. */
    writetty(serialfd, &"D", 1);
    for (size_t i = 0; i < record.length; ++i) {
      writetty(serialfd, &record.data[i], sizeof(uint8_t));
      uint8_t data = readtty(serialfd);
      if (data != record.data[i]) {
        fprintf(stderr, "bad data byte #" SSIZET_FMT ", expected %02x, got %02x\n", i, record.data[i], data);
        exit(1);
      } else {
        printf("%02x", data);
      }
    }

    /* Read the computer CRC and compare it to ours. */
    uint8_t crc = readtty(serialfd);
    if (crc != record.crc) {
      fprintf(stderr, "bad crc response, expected %02x, got %02x\n", record.crc, crc);
      exit(1);
    } else {
      printf("%02x", crc);
    }
    printf("\n");
  }

  /* Inform the other end we're finished. */
  writetty(serialfd, &"E", 1);
  printf("%s uploaded!\n", ihexFilePath);

  if (-1 == close(serialfd)) {
    perror("closing serial port\n");
  }

  if (-1 == close(fd)) {
    fprintf(stderr, "close %s: %s\n", ihexFilePath, strerror(errno));
  }
}
