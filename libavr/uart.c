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

#ifndef UCSR0A
#define UCSR0A UCSRA
#endif

#ifndef UCSR0B
#define UCSR0B UCSRB
#endif

#ifndef UCSR0C
#define UCSR0C UCSRC
#endif

#ifndef UBRR0H
#define UBRR0H UBRRH
#endif

#ifndef UBRR0L
#define UBRR0L UBRRL
#endif

#ifndef RXCIE0
#define RXCIE0 RXCIE
#endif

#ifndef UPE0
#define UPE0 UPE
#endif

#ifdef AVR_UART_ISR_RX_ENABLE
#include <avr/interrupt.h>
#define RX_BUFFER_SIZE        64
#define RX_BUFFER_OFFSET(i)   ((RX_BUFFER_SIZE-1) & (i))
typedef struct {
  char buffer[RX_BUFFER_SIZE];
  uint8_t head, tail;
} RingBuffer;

static RingBuffer rx_buffer;
#endif // AVR_UART_ISR_RX_ENABLE

static int uart0_putchar(char c, FILE* stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);

int uart0_putchar(char c, FILE* stream) {
  if ('\n' == c) {
    uart0_putchar('\r', stream);
  }

  uart0_transmit(c);
  return 0;
}

void uart0_setup_stdout() {
  stdout = &mystdout;
}

void uart0_enable(UARTMode syncMode) {
  static uint16_t baudrate = ((F_CPU / (16 * BAUD)) - 1);
  UBRR0H = baudrate >> 8;
  UBRR0L = baudrate;

  (void)UDR0; // clear any data currently in the buffer

  // Set RX/TN enabled
  UCSR0B |= _BV(TXEN0) | _BV(RXEN0);

#ifdef AVR_UART_ISR_RX_ENABLE
  UCSR0B |= _BV(RXCIE0);
#endif

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
  while ((UCSR0A & (_BV(UDRE0) | _BV(TXC0))) == 0);
  UDR0 = data;
}

#ifdef AVR_UART_ISR_RX_ENABLE
ISR(USART_RX_vect) {
}

uint8_t uart0_receive(uint8_t* bad_parity) {
  while (rx_buffer.head == rx_buffer.tail); // no data available

  uint8_t result = rx_buffer.head;
  rx_buffer.head = RX_BUFFER_OFFSET(rx_buffer.head+1);
  return result;
}
#else
uint8_t uart0_receive(uint8_t* bad_parity) {
  while (!(UCSR0A & _BV(RXC0)));
  if (bad_parity) {
    *bad_parity = UCSR0A & _BV(UPE0);
  }
  return UDR0;
}
#endif // AVR_UART_ISR_RX_ENABLE

void uart0_write(uint8_t* data, uint8_t length) {
  for (uint8_t i = 0; i < length; ++i) {
    uart0_transmit(data[i]);
  }
}
