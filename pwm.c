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
  ICR1 = us_clocks(10000, Prescaled_8);

  uart_enable(UM_Asynchronous);
  sei();

  volatile uint16_t* pin = 0;
  int usBuffer = 0;
  int doEcho = 0;
  for (;;) {
    unsigned char b = uart_receive();
    if (b >= '0' && b <= '9') {
      usBuffer *= 10;
      usBuffer += b-'0';
    } else if ('A' == b || 'a' == b) {
      pin = &OCR1A;
    } else if ('B' == b || 'b' == b) {
      pin = &OCR1B;
    } else if ('E' == b || 'e' == b) {
      doEcho = !doEcho;
    } else if ('\r' == b) {
      if (pin) {
        *pin = us_clocks(usBuffer, Prescaled_8);
        usBuffer = 0;
      }

      if (doEcho) {
        uart_transmit('\r');
        b = '\n';
      }
    }

    if (doEcho) {
      uart_transmit(b);
    }
  }

  return 0;
}
