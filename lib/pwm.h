/**
Also, the port and pin mapping of the output compare match 1A (or 1 for older devices) pin which is
used to drive the PWM varies between different AVRs.  This file tries to abstract between all these
differences using some preprocessor #ifdef statements, so the actual program itself can operate on a common set of
symbolic names. The macros defined by that file are:

 - OCR the name of the OCR register used to control the PWM (usually either OCR1 or OCR1A)
 - DDROC the name of the DDR (data direction register) for the OC output
 - OC1 the pin number of the OC1[A] output within its port
 - TIMER1_TOP the TOP value of the timer used for the PWM (1023 for 10-bit PWMs, 255 for devices that can only handle an 8-bit PWM)
 - TIMER1_PWM_INIT the initialization bits to be set into control register 1A in order to setup 10-bit (or 8-bit)
   phase and frequency correct PWM mode
 - TIMER1_CLOCKSOURCE the clock bits to set in the respective control register to start the PWM timer; usually the
   timer runs at full CPU clock for 10-bit PWMs, while it runs on a prescaled clock for 8-bit PWMs
*/

#pragma once

#include <math.h>

/**
 * Per table 16-4, set waveform generation to phase correct with ICR1 as TOP.
 */
typedef enum {
  WGM_CTC,
  WGM_FAST_PWM,
  WGM_PHASE_CORRECT_PWM,
  WGM_PHASE_AND_FREQUENCE_CORRECT_PWM,
} WaveGenerationMode;

/**
 * Per table 16-5, this is F_CPU / 8 (prescaling).
 */
typedef enum {
  CS_Prescaled_1 = 0,
  CS_Prescaled_8,
  CS_Prescaled_64,
  CS_Prescaled_256,
  CS_Prescaled_1024,
  
  CS_Disabled,
  CS_XTAL_Falling,
  CS_XTAL_Rising,
} ClockSource;

/**
 * Per table 16-2, set OC1A/B on compare match when up-counting and clear
 * on compare-match when down-counting.
 */
typedef enum {
  /* OC1A/OC1B disconnected.  Normal port operation. */
  OC_DISCONNECTED,
  
  /**
   * Non-PWM: Toggle OC1A/OC1B on compare match.
   * PWM: Toggle OC1A on compare match, OC1B disconnected.
   */
  OC_TOGGLE,

  /**
   * Non-PWM: Clear OC1A/OC1B on compare match.
   * Fast PWM: Clear OC1A/OC1B on compare match; set at BOTTOM.
   * PC/PFC PWM: Clear OC1A/OC1B on compare match while upcounting; set on compare match while downcounting.
   */
  OC_NON_INVERTING,

  /**
   * Non-PWM: Set OC1A/OC1B on compare match.
   * Fast PWM: Set OC1A/OC1B on compare match; clear at BOTTOM.
   * PC/PFC PWM: Set OC1A/OC1B on compare match while upcounting; clear on compare match while downcounting.
   */
  OC_INVERTING,
} OutputCompareMode;

// Data direction register managing output compare pins.
#define DDROC DDRB

// Output compare pins
#define OC1 PB1
#define OC2 PB2

/**
 * Setup 16-bit timer/counter's wave generation mode.
 * No matter what WGM mode is selected, ICR will act as TOP.
 */
static inline void wgm1(WaveGenerationMode mode) {
  TCCR1A &= 0xFF ^ (_BV(WGM11) | _BV(WGM10));
  TCCR1B &= 0xFF ^ (_BV(WGM13) | _BV(WGM12));
  
  switch (mode) {
  case WGM_CTC:
    TCCR1B |= _BV(WGM13) | _BV(WGM12);
    break;
  case WGM_FAST_PWM:
    TCCR1A |= _BV(WGM11);
    TCCR1B |= _BV(WGM13) | _BV(WGM12);
    break;
  case WGM_PHASE_CORRECT_PWM:
    TCCR1A |= _BV(WGM11);
    TCCR1B |= _BV(WGM13);
    break;
  case WGM_PHASE_AND_FREQUENCE_CORRECT_PWM:
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(WGM13);
    break;
  }
}

static inline void cs1(ClockSource cs) {
  TCCR1B &= 0xFF ^ (_BV(CS12) | _BV(CS11) | _BV(CS10));

  switch (cs) {
  case CS_Disabled:
    // First line already does a clear.
    break;
  case CS_Prescaled_1:
    TCCR1B |= _BV(CS10);
    break;
  case CS_Prescaled_8:
    TCCR1B |= _BV(CS11);
    break;
  case CS_Prescaled_64:
    TCCR1B |= _BV(CS11) | _BV(CS10);
    break;
  case CS_Prescaled_256:
    TCCR1B |= _BV(CS12);
    break;
  case CS_Prescaled_1024:
    TCCR1B |= _BV(CS12) | _BV(CS10);
    break;
  case CS_XTAL_Falling:
    TCCR1B |= _BV(CS12) | _BV(CS11);
    break;
  case CS_XTAL_Rising:
    TCCR1B |= _BV(CS12) | _BV(CS11) | _BV(CS10);
    break;
  }
}

static inline void oc1(OutputCompareMode com) {
  TCCR1A &= 0xFF ^ (_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));

  switch (com) {
  case OC_DISCONNECTED:
    break;
  case OC_TOGGLE:
    TCCR1A |= _BV(COM1A0) | _BV(COM1B0);
    break;
  case OC_NON_INVERTING:
    TCCR1A |= _BV(COM1A1) | _BV(COM1B1);
    break;
  case OC_INVERTING:
    TCCR1A |= _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0);
    break;
  }
}

#define MHZ 1000000
#if F_CPU < MHZ
#  error F_CPU must be >= 1000000
#endif

// Computation for a single microsecond, rounded down to nearest microsecond.
// So, it is only as correct as the fractional difference.
static inline int us_clocks(int us, ClockSource cs) {
  int clocks = us * MHZ / F_CPU;
  if (cs > CS_Prescaled_1024) {
    return -1;
  }

  return clocks << cs;
}
