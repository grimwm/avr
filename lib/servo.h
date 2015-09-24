#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/interrupt.h>

#include "pwm.h"

#define PWM_MIN_US 500
#define PWM_MAX_US 1000

/**
 * Generate PWM duty cycle for servo positions between 0 and 180 degrees.
 */
static inline unsigned servo(unsigned degrees) {
  unsigned us = PWM_MIN_US + ((PWM_MAX_US - PWM_MIN_US) * degrees / 180);
  return us_clocks(us, Prescaled_8);
}

/**
 * Assume 50 Hz (20 ms) initialization.
 * Servos will be set to 90 degrees.
 * Don't forget to call sei() after you initialize hardware!
 */
static inline void servo0_init(unsigned out_pins) {
  cs0(Prescaled_8);
  wgm0(FastPWM);
  oc0(NonInverting);

  oc0_enable(out_pins);

  // Set top of counter to 200us.  Combined with clever ISRs, this
  // can easily be used to line up with 20ms = 50 Hz.
  OCR0A = us_clocks(200, Prescaled_8);

  // Set servos to 90 degrees.
//  servo(90, &OCR1A);
//  servo(90, &OCR1B);
}

/**
 * Assume 50 Hz (20 ms) initialization.
 * Servos will be set to 90 degrees.
 * Don't forget to call sei() after you initialize hardware!
 */
static inline void servo1_init(unsigned out_pins) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);

  oc1_enable(out_pins);

  // Set top of counter to 10000us, setting a period of 20ms = 50 Hz.
  ICR1 = us_clocks(10000, Prescaled_8);

  // Set servos to 90 degrees.
  OCR1A = OCR1B = servo(90);
}

/**
 * If INSTALL SERVO0_ISRS is set, install a set of ISRs for managing
 * 50 Hz servo signals at the proper duty cycles.  use the associated
 * non-ISR methods to set the appropriate registers for the desired
 * duty cycles.
 */
#ifdef INSTALL_SERVO0_ISRS
extern volatile uint8_t x200us;
/**
 * Set this up with the assumption we have a TOP of 200,
 * with each tick counting as 1us.  So, when this has executed
 * exactly 100 times, it will have been 100*200us = 20ms or 50 Hz,
 * enough for a servo signal period.
 */
ISR(TIMER0_OVF_vect) {
  ++x200us;
  if (100 == x200us) { // clock-cycle efficient
    // We've reached 20ms and need to reset.
    x200us = 0;
  }
}

ISR(TIMER0_COMPA_vect) {
}

ISR(TIMER0_COMPB_vect) {
}
#endif // INSTALL_SERVO0_ISRS

#ifdef __cplusplus
} // extern "C"
#endif
