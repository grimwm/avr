#include <avr/interrupt.h>

#include "uart.h"

int main (void) {
  uart_enable(UM_Asynchronous);
  sei();

  unsigned char c = 0;
  do {
    c = uart_receive();
    uart_transmit(c);
  } while (c);

  return 0;
}

