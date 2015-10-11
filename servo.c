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

#define NACK_BYTE 0xFF

#define NUM_RESET_BYTES 4
#define RESET_BYTE(x) (0xFF-(x))

#define LEFT 'L'
#define RIGHT 'R'

int main (void) {
  servo_init(OC1A | OC1B);
  uart_enable(UM_Asynchronous);
  sei();

  /*
   * Initialize comms by making sure serial device is sending proper data.
   */
  for (int i = 0; i < NUM_RESET_BYTES;) {
    unsigned char b = uart_receive();
    if (RESET_BYTE(i) == b) {
      ++i;
    } else {
      i = 0;
    }
  }

  /*
   * Run a loop that receives a command and controls the servos with it.
   * If any of the commands don't specify which servo to control, send
   * back a NACK_BYTE; otherwise, send back the msgid.
   */
  for (;;) {
    unsigned char msgid = uart_receive();
    unsigned char cmd = uart_receive();
    uint16_t value;
    value = uart_receive() << 8;
    value |= uart_receive();

    if (LEFT == cmd) {
      OCR1A = servo(value);
    } else if (RIGHT == cmd) {
      OCR1B = servo(value);
    } else {
      uart_transmit(NACK_BYTE);
      continue;
    }
    
    uart_transmit(msgid);
  }

  return 0;
}
