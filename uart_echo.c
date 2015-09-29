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

