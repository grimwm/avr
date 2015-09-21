#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "servo.h"
#include "spi.h"

#define ACK 0x7E

int main (void) {
  servo_init(OC1A | OC1B);
  spi_init_master(0);
  /* spi_init_slave(0); */
  sei();

  /* unsigned char degrees = 90; */
  for (;;) {
  /*   sleep_mode(); // sleep until we're awoken by an interrupt */
    for (unsigned char degrees = 0; degrees <= 180; ++degrees) {
    servo(degrees, &OCR1A);
    servo(degrees, &OCR1B);

    /* _delay_ms(1000); */

    degrees = spi_tranceiver(ACK);
    }
  }

  return 0;
}
