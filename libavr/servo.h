/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#include "pwm.h"

#define PWM_MIN_US 500U   // 1ms with Phase Correct PWM
#define PWM_MAX_US 1000U  // 2ms with Phase Correct PWM

/**
 * Generate PWM duty cycle for servo positions between 0 and 180 degrees.
 * Assumes a duty cycle of 1ms for the 0 degree position and
 * 2ms for the 180 degree position.
 */
static inline unsigned servo(unsigned degrees) {
  static const unsigned m = (PWM_MAX_US - PWM_MIN_US) / 2;
  const unsigned us = m + (m * degrees / 180);
  return us_clocks(us, Prescaled_8);
}

/**
 * Assume 50 Hz (20 ms) initialization.
 * Servos will be set to 90 degrees.
 * Don't forget to call sei() after you initialize hardware!
 */
static inline void servo_init(unsigned out_pins, unsigned degrees) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);

  // Set top of counter to 10000us, setting a period of 20ms = 50 Hz.
  ICR1 = us_clocks(10000, Prescaled_8);

  // Set servos to degrees.
  OCR1A = OCR1B = servo(degrees);

  oc1_enable(out_pins);
}

#ifdef __cplusplus
} // extern "C"
#endif
