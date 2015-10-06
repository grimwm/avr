#include "uart.h"

/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 *
 * This is used to setup the UART device on an AVR unit.
 */

#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>

static int uart_putchar(unsigned char c, FILE* stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int uart_putchar(unsigned char c, FILE* stream) {
  if ('\n' == c) {
    uart_putchar('\n', stream);
  }

  uart_transmit(c);
  return 0;
}

void uart_setup_stdout() {
  stdout = &mystdout;
}

void uart_enable(UARTMode syncMode) {
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  (void)UDR0; // clear any data currently in the buffer

  // Set RX/TN enabled
  UCSR0B |= _BV(TXEN0) | _BV(RXEN0);

  // Set frame format: 8-bit data, 1 stop bit
  UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
  switch (syncMode) {
  case UM_Asynchronous:
    break;
  case UM_Synchronous:
    UCSR0C |= _BV(UMSEL00);
    break;
  case UM_MasterSPI:
    UCSR0C |= _BV(UMSEL01) | _BV(UMSEL00);
    break;
  }
}

void uart_transmit(unsigned char data) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
}

unsigned char uart_receive(void) {
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}
