#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "pwm.h"

#define PWM_MIN_US 500
#define PWM_MAX_US 1000

static inline unsigned usc(unsigned us) {
  return us_clocks(us, Prescaled_8);
}

/**
 * This routine gets called after a reset. It initializes the PWM and enables interrupts.
 **/
void timerinit(void) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);

  OC1_ENABLE(OC1A | OC1B);

  // Set top of counter to 1500us, setting a period of 20000us = 50 Hz.
  ICR1 = usc(10000);

  // Set PWM duty cycle to 1500us.
  OCR1A = OCR1B = usc(750);

  sei();
}

static inline unsigned lte(unsigned a, unsigned b) {
  return a <= b;
}

static inline void setOC(unsigned m1, unsigned m2, unsigned us) {
  OCR1A = usc(m1 + m2 - us);
  OCR1B = usc(us);
}

int main (void) {
  timerinit();

  unsigned m1 = PWM_MIN_US;
  unsigned m2 = PWM_MAX_US;

  for (;;) {
  /*   sleep_mode(); // sleep until we're awoken by an interrupt */

    for (unsigned us=m1; lte(us, m2); ++us) {
      _delay_ms(1);
      setOC(m1, m2, us);
    }

    for (unsigned us=m2; lte(m1, us); --us) {
      _delay_ms(1);
      setOC(m1, m2, us);
    }
  }

  return 0;
}
