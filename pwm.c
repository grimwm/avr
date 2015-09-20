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
  OCR1A = OCR1B = us_clocks(500, cs);
  
  sei();
}

int main (void) {
  timerinit();
  
  /* loop forever, the interrupts are doing the rest */
  /* volatile unsigned *a = &OCRA; */
  /* volatile unsigned int *b = &OCRB; */
  for (;;) {
  /*   sleep_mode(); // sleep until we're awoken by an interrupt */
    /* for (int us = 0; us < 1500; ++us) { */
    /*   _delay_ms(1); */
    /*   *a = us_clocks(1500 - us); */
    /*   *b = us_clocks(us); */
    /* } */

    /* volatile unsigned *t = a; */
    /* a = b; */
    /* b = t; */
  }
  return (0);
}
