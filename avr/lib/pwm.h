/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 *
 * This file does its best to help you setup a 16-bit timer/counter with
 * ICR1 as TOP.  Currently, any other modes of these two timer/counters
 * are unsupported.  Any other timer/counters are also not supported at
 * this time.
 *
 * The work of setting up the timer/counters is left mainly to three functions:
 *  - cs1(): clock source setup
 *  - wgm1(): wave generation mode setup
 *  - oc1(): output compare mode setup
 *
 * ICR1's valid TOP ranges are between 0x00 and 0xFFFF.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#include "clock.h"

// Output compare pins
#define OC1A _BV(PB1)
#define OC1B _BV(PB2)

// Convenience methods to setup pins.
#define oc_enable(ddr, pins) ddr |= (pins)
#define oc_disable(ddr, pins) ddr &= ~(pins)
#define oc_toggle(ddr, pins) ddr ^= (pins)

#define oc1_enable(pins) oc_enable(DDRB, pins)
#define oc1_disable(pins) oc_disable(DDRB, pins)
#define oc1_toggle(pins) oc_toggle(DDRB, pins)

/**
 * wgm1 functions have their masks taken from table 16-4.  In all the modes
 * available here, ICR1 acts as TOP.
 *
 * Setup 16-bit timer/counter's wave generation mode.
 * No matter what WGM mode is selected, ICR will act as TOP.
 */
#define wgm1(mode) wgm1_ ## mode()
#define wgm1_clearA() TCCR1A = (TCCR1A & ~(_BV(WGM11) | _BV(WGM10)))
#define wgm1_clearB() TCCR1B = (TCCR1B & ~(_BV(WGM13) | _BV(WGM12)))
#define wgm1_CTC() \
  wgm1_clearA(); \
  wgm1_clearB() | _BV(WGM13) | _BV(WGM12);
#define wgm1_FastPWM() \
  wgm1_clearA() | _BV(WGM11); \
  wgm1_clearB() | _BV(WGM13) | _BV(WGM12);
#define wgm1_PhaseCorrectPWM() \
  wgm1_clearA() | _BV(WGM11); \
  wgm1_clearB() | _BV(WGM13);
#define wgm1_PhaseAndFrequencyCorrectPWM() \
  wgm1_clearA() | _BV(WGM10); \
  wgm1_clearB() | _BV(WGM13);

#define cs1(mode) cs1_ ## mode()
#define cs1_clear() TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11) | _BV(CS10)))
#define cs1_Disabled() cs1_clear()
#define cs1_Prescaled_1() cs1_clear() | _BV(CS10)
#define cs1_Prescaled_8() cs1_clear() | _BV(CS11)
#define cs1_Prescaled_64() cs1_clear() | _BV(CS11) | _BV(CS10)
#define cs1_Prescaled_256() cs1_clear() | _BV(CS12)
#define cs1_Prescaled_1024() cs1_clear() | _BV(CS12) | _BV(CS10)
#define cs1_XTAL_Falling() cs1_clear() | _BV(CS12) | _BV(CS11)
#define cs1_XTAL_Rising() cs1_clear() | _BV(CS12) | _BV(CS11) | _BV(CS10)

/**
 * Per table 16-2, set OC1A/B on compare match when up-counting and clear
 * on compare-match when down-counting.
 */
#define oc1(mode) oc1_ ## mode()
#define oc1_Disconnected() TCCR1A = (TCCR1A & ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0)))
#define oc1_Toggle() oc1_Disconnected() | _BV(COM1A0) | _BV(COM1B0)
#define oc1_NonInverting() oc1_Disconnected() | _BV(COM1A1) | _BV(COM1B1)
#define oc1_Invertibg() oc1_Disconnected() | _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0)

#ifdef __cplusplus
} // extern "C"
#endif
