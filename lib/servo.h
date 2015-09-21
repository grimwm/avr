#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "pwm.h"

#define PWM_MIN_US 500
#define PWM_MAX_US 1000

/**
 * Generate PWM for servo positions between 0 and 180 degrees.
 */
static inline void servo(unsigned degrees, volatile uint16_t* pin) {
  unsigned us = PWM_MIN_US + ((PWM_MAX_US - PWM_MIN_US) * degrees / 180);
  *pin = us_clocks(us, Prescaled_8);
}

/**
 * Assume 50 Hz (20 ms) initialization.
 * Servos will be set to 90 degrees.
 * Don't forget to call sei() after you initialize hardware!
 */
static inline void servo_init(unsigned out_pins) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);

  OC1_ENABLE(out_pins);

  // Set top of counter to 1500us, setting a period of 20000us = 50 Hz.
  ICR1 = us_clocks(10000, Prescaled_8);

  // Set servos to 90 degrees.
  servo(90, &OCR1A);
  servo(90, &OCR1B);
}

#ifdef __cplusplus
} // extern "C"
#endif
