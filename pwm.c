#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>

#include "servo.h"
#include "pwm.h"
#include "uart.h"

int main (void) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);
  oc1_enable(OC1A | OC1B);

  uart_enable(UM_Asynchronous);
  sei();

  /* volatile uint16_t* pin = 0; */
  uint16_t dutyBuffer = 0;
  int doEcho = 0;
  for (;;) {
    unsigned char b = uart_receive();
    if (b >= '0' && b <= '9') {
      dutyBuffer *= 10;
      dutyBuffer += b-'0';
    }

    if ('\r' == b) {
      /* if (pin) { */
      /*   *pin = dutyBuffer; */
      /* } */
      OCR1A = dutyBuffer;
      while (dutyBuffer > 0) {
        b = (dutyBuffer % 10) + '0';
        dutyBuffer /= 10;
      }
      b = '\r';
    /* } else if ('A' == b) { */
    /*   pin = &OCR1A; */
    /* } else if ('B' == b) { */
    /*   pin = &OCR1B; */
    } else if ('E' == b || 'e' == b) {
      doEcho = !doEcho;
    }

    if (doEcho) {
      uart_transmit(b);
    }
  }

  return 0;
}
