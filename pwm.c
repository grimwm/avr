#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#include "servo.h"
#include "uart.h"

#define ACK 0x7E

// Memory for the ISR
volatile uint8_t x200us;

int main (void) {
//  servo0_init(OC0A | OC0B);
//  spi_init_slave(0);
  uart_enable();
  sei();

  /* unsigned char degrees = 90; */
  for (;;) {
  /*   sleep_mode(); // sleep until we're awoken by an interrupt */
    for (unsigned char degrees = 0; degrees <= 180; ++degrees) {
      _delay_ms(1000);
    }
  }

  return 0;
}
