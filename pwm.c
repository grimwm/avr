/**
 * This program sets up a 50 Hz PWM and a basic UART command structure.
 * Users may connect to the UART and send one byte at a time to make up
 * more complex commands.  The basic layout of the command format is
 * like this: [B_1,B_2,B_3,B_4,...,B_n,'\r'], with the commas, brackets,
 * and quotes all being abstract (e.g. not really sent across the wire).
 *
 * The commands respond immediately, not having an "end" until the CR
 * gets sent, which will setup the buffers for the output pins on the PWM.
 *
 * Some example commands:
 * -  EA1000<ENTER>     Toggles character echo, sets OCR1A's duty cycle to 1000us
 * -  200B<ENTER>       Sets OCR1B's duty cycle to 200us.
 * -  E                 Toggle character echo.
 */
#include <inttypes.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "pwm.h"
#include "uart.h"

uint16_t us2clocks(uint16_t us) {
  return us_clocks(us, Prescaled_8);
}

int main (void) {
  cs1(Prescaled_8);
  wgm1(PhaseCorrectPWM);
  oc1(NonInverting);
  oc1_enable(OC1A | OC1B);
  ICR1 = us_clocks(10000, Prescaled_8);

  uart_enable(UM_Asynchronous);
  sei();

  volatile uint16_t* pin = 0;
  uint16_t usBuffer = 0;
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
    } else if ('\r' == b || '\n' == b) {
      if (pin) {
        *pin = us2clocks(usBuffer);
        usBuffer = 0;
      }

      if (doEcho && '\r' == b) {
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
