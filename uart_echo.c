/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"

int main (void) {
  uart_enable(UM_Asynchronous);
  sei();

  DDRB = 0xFF;

  unsigned char c = 0;
  do {
    c = uart_receive();

    // Do some blinking if we receive a number between 1 and 9.
    if (c >= '1' && c <= '9') {
      for (int i = 0; i < (c-'0'); ++i) {
        PORTB = 0xFF;
        _delay_ms(100);
        PORTB = 0x00;
        _delay_ms(100);
      }
    }
    
    uart_transmit(c);
  } while (1);

  return 0;
}

