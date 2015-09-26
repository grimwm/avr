#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#include "servo.h"
#include "uart.h"

int main (void) {
//  servo_init(OC1A | OC1B);
  uart_enable(UM_Asynchronous);
  sei();

//  unsigned char degrees = 90;
  for (;;) {
  /*   sleep_mode(); // sleep until we're awoken by an interrupt */
    for (unsigned char degrees = 0; degrees <= 180; ++degrees) {
      _delay_ms(1000);
    }
  }

  return 0;
}
