#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "pwm.h"

static ClockSource cs = CS_Prescaled_8;
static WaveGenerationMode wgm = WGM_PHASE_CORRECT_PWM;
static OutputCompareMode com = OC_NON_INVERTING;

/**
 * This routine gets called after a reset. It initializes the PWM and enables interrupts.
 **/
void timerinit(void) {
  cs1(cs);
  wgm1(wgm);
  oc1(com);

  // Enable OC1 as output.
  DDROC = _BV(OC1) | _BV(OC2);

  // Set top of counter to 1500us, setting a period of 20000us = 50 Hz.
  ICR1 = us_clocks(10000, cs);

  // Set PWM duty cycle to 1500us.
  OCR1A = OCR1B = us_clocks(750, cs);
  
  sei();
}

const unsigned PWM_RANGE_MICROSECONDS[2] = {
  500,                          /* MIN */
  1000,                         /* MAX */
};

int lte(int a, int b) {
  return a <= b;
}

int gte(int a, int b) {
  return a >= b;
}

typedef int (*LessThanEqual)(int,int);

int main (void) {
  timerinit();

  unsigned m1 = PWM_RANGE_MICROSECONDS[0];
  unsigned m2 = PWM_RANGE_MICROSECONDS[1];
  
  LessThanEqual cmpa = &lte;
  LessThanEqual cmpb = &gte;

  for (int increment=1, us=m1; ; increment *= -1) {
  /*   sleep_mode(); // sleep until we're awoken by an interrupt */

    for (; cmpa(us, m2); us += increment) {
      _delay_ms(1);
      OCR1A = us_clocks(m1 + m2 - us, cs);
      OCR1B = us_clocks(us, cs);
    }

    LessThanEqual cmpt = cmpa;
    cmpa = cmpb;
    cmpb = cmpt;

    m1 ^= m2;
    m2 ^= m1;
    m1 ^= m2;
  }
  return (0);
}
