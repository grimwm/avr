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

#define ACK_BYTE 0xFF

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

  /**
   * Packet structure: 1b (servo A or B) 16b (duty cycle in us)
   */
  volatile uint16_t* pin = 0;
  volatile uint16_t usBuffer = 0;
  for (;;) {
    unsigned char b = uart_receive();
    
    if (0xFF == b) {
      usBuffer = 0;
      OCR1A = OCR1B = 0;
      continue;
    }

    if ('A' == b) {
      pin = &OCR1A;
    } else {
      pin = &OCR1B;
    } 

    b = uart_receive();
    usBuffer = b << 8;
    b = uart_receive();
    usBuffer |= b;
    
    *pin = us2clocks(usBuffer);
    b = ACK_BYTE;
    usBuffer = 0;

    uart_transmit(b);
  }

  return 0;
}
