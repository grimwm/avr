/**
 * This program sets up a 50 Hz PWM and a basic UART command structure.
 * Users may connect to the UART and send one byte at a time to make up
 * more complex commands.  The basic layout of the command format is
 * like this: [B_1,B_2,0xFE], with the commas, brackets, and quotes all
 * being abstract (e.g. not really sent across the wire).
 *
 * B_1 will generally be the servo of interest (e.g. 'A' or 'B').
 * B_2 will generally be a byte between 0 and 180, representing degrees.
 * B_1 and B_2 may be sent interchangeably or even many times over, and the
 * AVR will just continue to record the data and echo bytes received back
 * to the sender until a COMMAND_EXEC_BYTE is reached.
 *
 * Bytes sent across the wire will be echoed back to the sending device,
 * except in the case when COMMAND_EXEC_BYTE (0xFE) is received.  In that
 * case, ACK_BYTE (0xFF) will be sent back to the sending device.
 *
 * In the case where the AVR thinks you are trying to set the degrees and
 * the degrees are outside the range of [0,180], it will send back a BAD_BYTE.
 */
#include <inttypes.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "servo.h"
#include "uart.h"

#define COMMAND_EXEC_BYTE 0xFD
#define BAD_BYTE 0xFE
#define ACK_BYTE 0xFF

int main (void) {
  servo_init(OC1A | OC1B);
  uart_enable(UM_Asynchronous);
  sei();

  volatile uint16_t* pin = 0;
  uint8_t degrees = 0;
  for (;;) {
    volatile unsigned char b = uart_receive();
    switch (b) {
    case COMMAND_EXEC_BYTE:
      *pin = servo(degrees);
      b = ACK_BYTE;
      break;
    case 'A':
      pin = &OCR1A;
      break;
    case 'B':
      pin = &OCR1B;
      break;
    case ACK_BYTE:
      break;
    default:
      if (b > 180) {
        b = BAD_BYTE;
      } else {
        degrees = b;
      }
      break;
    }
    
    uart_transmit(b);
  }

  return 0;
}
