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

static int uart0_putchar(char c, FILE* stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);

int uart0_putchar(char c, FILE* stream) {
  if ('\r' == c) {
    uart0_putchar('\n', stream);
  }

  uart0_transmit(c);
  return 0;
}

void uart0_setup_stdout() {
  stdout = &mystdout;
}

void uart0_enable(UARTMode syncMode) {
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

void uart0_transmit(uint8_t data) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
}

uint8_t uart0_receive_buffer_full(void) {
  return UCSR0A & _BV(RXC0);
}

uint8_t uart0_receive(void) {
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

void uart0_write(uint8_t* data, uint8_t length) {
  for (uint8_t i = 0; i < length; ++i) {
    uart0_transmit(data[i]);
  }
}
