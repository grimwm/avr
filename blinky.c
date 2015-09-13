#define F_CPU 10000000UL

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
  DDRD = 0xFF;  // PORTB is output, all pins
  PORTD = 0x00; // Make pins low to start

  // blink an LED for S.O.S.
  for (;;) {
    PORTD ^= 0xFF;   // invert all the pins
    for (int i = 0; i < 3; ++i) {
      PORTD = 0xFF;
      _delay_ms(200);
      PORTD = 0x00;
      _delay_ms(200);
    }

    for (int i = 0; i < 3; ++i) {
      PORTD = 0xFF;
      _delay_ms(1000);
      PORTD = 0x00;
      _delay_ms(200);
    }
  }

  return 0;
}

