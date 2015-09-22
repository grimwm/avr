#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "servo.h"
#include "spi.h"

#define ACK 0x7E

// These servo ISRs will be moved into servo.h, hidden behind a
// macro definition.
extern volatile int8_t x200us;
volatile int8_t x200us;
/**
 * Set this up with the assumption we have a TOP of 200,
 * with each tick counting as 1us.  So, when this has executed
 * exactly 100 times, it will have been 100*200us = 20ms or 50 Hz,
 * enough for a servo signal period.
 */
ISR(TIMER0_OVF_vect) {
  ++x200us;
  if (100 == x200us) { // clock-cycle efficient
    x200us = 0;
  }
}

ISR(TIMER0_COMPA_vect) {
}

ISR(TIMER0_COMPB_vect) {
}

int main (void) {
  servo0_init(OC1A | OC1B);
  spi_init_slave(0);
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
