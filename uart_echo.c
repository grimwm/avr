#include <avr/interrupt.h>

#include "uart.h"

int main (void) {
  uart_enable(UM_Asynchronous);
  sei();

  unsigned char c = 0;
  do {
    /*   sleep_mode(); // sleep until we're awoken by an interrupt */
  } while (c);

  return 0;
}

