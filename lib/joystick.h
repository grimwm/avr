/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#pragma once

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JoystickOptions {
  char devicepath[PATH_MAX];
  char y_left;    // y-axis on left joystick
  char y_right;   // y-axis on right joystick
} JoystickOptions;

typedef struct Joystick {
  int fd;
  int driverVersion;
  char name[128];
  char naxes;
  char nbuttons;
} Joystick;

/*
 * The kinds of {@see JoystickEvent} events that can occur.
 */
#define JSE_BUTTON  0x01  /* button pressed/released */
#define JSE_AXIS    0x02  /* joystick moved */
#define JSE_INIT    0x80  /* initial state of device */

/*
 * Modeled after Linux's jsevent, this contains information
 * about a joystick event.
 */
typedef struct JoystickEvent {
  uint32_t time;        /* event timestamp in milliseconds */
  int16_t value;        /* value */
  unsigned char type;   /* event type */
  unsigned char number; /* axis/button number */
} JoystickEvent;

/**
 * @brief Create and return a set of joystick options.
 * @param devicePath
 * @param optionsPath
 * @return Options for the joystick.
 */
JoystickOptions JoystickOptions_init(const char* devicePath, const char* optionsPath);

/**
 * @brief Open a joystick device using the given "opts".
 * @param opts
 * @return Information about the opened joystick.
 */
Joystick Joystick_open(const JoystickOptions* opts);

/**
 * @brief Gets an event from the joystick defined in "js".
 * @param js The joystick device handle.
 * @param event
 * @return 1 if there was an event, 0 if there was no event, and -1 on error.
 */
int Joystick_getEvent(const Joystick* js, JoystickEvent* event);

#ifdef __cplusplus
} // extern "C"
#endif
