#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>

#include "test.h"

int main(void)
{
  DDRB = 0xFF; // PORTB is output, all pins
  PORTB = 0x00; // Make pins low to start

  if (TESTLIB_VERSION == testlib_version()) {
    // blink an LED
    for (;;) {
      PORTB ^= 0xFF; // invert all the pins
      _delay_ms(100); // wait some time
    }
  }
  else {
    // set LED always on
  }

  return 0;
}

