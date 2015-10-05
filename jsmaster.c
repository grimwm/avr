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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>

#include <linux/joystick.h>

typedef struct JoystickOptions {
  char device[PATH_MAX];
} JoystickOptions;

#ifndef DEFAULT_JOYSTICK_DEVICE
#  define DEFAULT_JOYSTICK_DEVICE "/dev/input/js0"
#endif

void JoystickOptions_init(JoystickOptions* opts) {
  strncpy(opts->device, DEFAULT_JOYSTICK_DEVICE, PATH_MAX);
}

static void pabort(const char *s) {
  perror(s);
  abort();
}

void print_usage(const char *prog) {
  printf("Usage: %s [-Db25678e]\n", prog);
  puts("  -D --device   device to use (default /dev/ttyAMA0)\n");
  exit(1);
}

JoystickOptions parse_opts(int argc, char *argv[]) {
  JoystickOptions options;
  JoystickOptions_init(&options);
  
  static const struct option lopts[] = {
    { "device",  1, 0, 'D' },
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
    }
  }

  return options;
}

int main(int argc, char* argv[]) {
  JoystickOptions opts = parse_opts(argc, argv);
  
  int fd = open(opts.device, O_RDONLY | O_NONBLOCK);
  if (-1 == fd) {
    pabort("can't open device");
  }

  int version;
  if (-1 == ioctl(fd, JSIOCGVERSION, &version)) {
    pabort("Error getting driver version");
  }

  char name[128];
  if (-1 == ioctl(fd, JSIOCGNAME(sizeof(name)), name)) {
    pabort("Error getting joystick name");
  }

  unsigned char naxes;
  if (-1 == ioctl(fd, JSIOCGAXES, &naxes)) {
    pabort("Error getting number joystick axes");
  }

  unsigned char nbuttons;
  if (-1 == ioctl(fd, JSIOCGBUTTONS, &nbuttons)) {
    pabort("Error getting number joystick buttons");
  }

  printf("Driver version: %d.%d.%d\n",
         version >> 16, (version >> 8) & 0xff, version & 0xff);
  printf("{\n"
         "\tName: %s\n"
         "\t# axes: %d\n"
         "\t# buttons: %d\n"
         "}\n",
         name, naxes, nbuttons);

  int* axes;
  char* buttons;

  axes = calloc(naxes, sizeof(int));
  buttons = calloc(nbuttons, sizeof(char));
  
  while (1) {
    struct js_event jsevent;

    /* fcntl(fd, F_SETFL, O_NONBLOCK); */
    while (read(fd, &jsevent, sizeof(struct js_event)) ==
           sizeof(struct js_event)) {
      switch (jsevent.type & ~JS_EVENT_INIT) {
      case JS_EVENT_AXIS:
        axes[jsevent.number] = jsevent.value;
        break;
      case JS_EVENT_BUTTON:
        buttons[jsevent.number] = jsevent.value;
        break;
      }

      printf("\r");
      
      // Print axis info
      printf("Axes: ");
      if (naxes) {
        for (unsigned char i = 0; i < naxes; ++i) {
          printf("%2d:%6d ", i, axes[i]);
        }
      }

      // Print button info
      printf("Buttons: ");
      if (nbuttons) {
        for (unsigned char i = 0; i < nbuttons; ++i) {
          printf("%2d:%s ", i, buttons[i] ? "on" : "off");
        }
      }

      usleep(100);
    }

    if (EAGAIN != errno) {
      pabort("Error getting js event");
    }
    
    fflush(stdout);
  }
}
