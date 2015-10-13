/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 *
 * This code sets up a 50 Hz phase-correct PWM and modifies the duty
 * cycle to be between 0ms and 4ms, which is ideal for testing whether
 * or not a pair of connected LEDs are pulsing.
 */
#include <inttypes.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "pwm.h"

#define PWM_MIN_DUTY_US         0
#define PWM_MAX_DUTY_US         2000
#define PWM_CYCLE_US            10000

uint16_t us2clocks(uint16_t us) {
  return us_clocks(us, Prescaled_8);
}

int main (void) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);
  oc1_enable(OC1A | OC1B);
  
  ICR1 = us2clocks(PWM_CYCLE_US);

  sei();

  OCR1A = us2clocks(PWM_MIN_DUTY_US);
  OCR1B = us2clocks(PWM_MAX_DUTY_US);
  for (;;) {
    for (int i = PWM_MIN_DUTY_US; i <= PWM_MAX_DUTY_US; ++i) {
      OCR1A = us2clocks(i);
      OCR1B = us2clocks(PWM_MAX_DUTY_US-i);
      _delay_ms(1);
    }
    for (int i = PWM_MAX_DUTY_US-1; i >= PWM_MIN_DUTY_US; --i) {
      OCR1A = us2clocks(i);
      OCR1B = us2clocks(PWM_MAX_DUTY_US-i);
      _delay_ms(1);
    }
  }

  return 0;
}
