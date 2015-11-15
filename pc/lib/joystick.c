/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include "joystick.h"
#include "io.h"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <linux/joystick.h>

#include <json/json_tokener.h>
#include <json/json_object.h>

json_object* jsonGetObject(json_object* obj, const char* key) {
  json_object* value;
  if (!json_object_object_get_ex(obj, key, &value)) {
    fprintf(stderr, "%s not in json", key);
    abort();
  }
  return value;
}

int jsonParseInt(json_object* obj, const char* key) {
  json_object* o = jsonGetObject(obj, key);
  int v = json_object_get_int(o);
  return v;
}

JoystickOptions JoystickOptions_init(const char* devicePath, const char* optionsPath) {
  int fd = open(optionsPath, O_RDONLY);
  if (-1 == fd) {
    pabort("cannot open file");
  }

  size_t filelen = filesize(fd);
  char* data = malloc(filelen);
  assert(data);
  if (-1 == read(fd, data, filelen)) {
    pabort("reading file");
  }
  assert(-1 != close(fd));

  json_tokener* tok = json_tokener_new();
  json_object* obj = json_tokener_parse(data);
  if (NULL == obj) {
    enum json_tokener_error err = json_tokener_get_error(tok);
    fprintf(stderr, "parsing joystick options: %s", json_tokener_error_desc(err));
    abort();
  }
  json_tokener_free(tok);
  free(data);

  JoystickOptions opts;
  if (!strncpy(opts.devicepath, devicePath, strlen(devicePath))) {
    perror("copying device path");
    abort();
  }

  opts.y_left = jsonParseInt(obj, "y_left");
  opts.y_right = jsonParseInt(obj, "y_right");
  free(obj);

  return opts;
}

Joystick Joystick_open(const JoystickOptions* opts) {
  Joystick js;
  js.fd = open(opts->devicepath, O_RDONLY | O_NONBLOCK);
  if (-1 == js.fd) {
    pabort("can't open device");
  }

  if (-1 == ioctl(js.fd, JSIOCGVERSION, &js.driverVersion)) {
    pabort("Error getting driver version");
  }

  if (-1 == ioctl(js.fd, JSIOCGNAME(sizeof(js.name)), js.name)) {
    pabort("Error getting joystick name");
  }

  if (-1 == ioctl(js.fd, JSIOCGAXES, &js.naxes)) {
    pabort("Error getting number joystick axes");
  }

  if (-1 == ioctl(js.fd, JSIOCGBUTTONS, &js.nbuttons)) {
    pabort("Error getting number joystick buttons");
  }

  return js;
}

int Joystick_getEvent(const Joystick* js, JoystickEvent* event) {
  while (1) {
    struct js_event jsevent;

    if (read(js->fd, &jsevent, sizeof(struct js_event)) == sizeof(struct js_event)) {
      event->number = jsevent.number;
      event->time = jsevent.time;
      event->type = jsevent.type;
      event->value = jsevent.value;
      return 1;
    }

    return EAGAIN == errno ? 0 : -1;
  }
}
