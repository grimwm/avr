/**
 * This is used to setup the UART device on an AVR unit.
 */

#pragma once

#include <avr/io.h>

#ifndef BAUD
#  define BAUD 9600
#  warning BAUD rate not set.  Setting BAUD rate to 9600.
#endif // BAUD

#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

static inline void uart_enable(void) {
  UBRR0H = BAUDRATE >> 8;
  UBRR0L = BAUDRATE;
  UCSR0B |= _BV(TXEN0) | _BV(RXEN0);
  UCSR0C |= _BV(UMSEL00) | _BV(UCSZ01) | _BV(UCSZ00);
}

static inline void uart_transmit(unsigned char data) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
}

static inline unsigned char uart_receive(void) {
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}
