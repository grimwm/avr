/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

/* #include <stdlib.h> */
#include <avr/io.h>
#include <util/delay.h>

int main(void) {
  DDRB = 0xFF;  // PORTB is output, all pins
  PORTB = 0x00; // Make pins low to start

  // blink an LED for S.O.S.
  for (;;) {
    for (int i = 0; i < 3; ++i) {
      PORTB = 0xFF;
      _delay_ms(200);
      PORTB = 0x00;
      _delay_ms(200);
    }

    for (int i = 0; i < 3; ++i) {
      PORTB = 0xFF;
      _delay_ms(1000);
      PORTB = 0x00;
      _delay_ms(200);
    }
  }

  return 0;
}

