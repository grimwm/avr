/**
 * This is used to setup the UART device on an AVR unit.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#ifndef BAUD
#  define BAUD 9600
#  warning BAUD rate not set.  Setting BAUD rate to 9600.
#endif // BAUD

#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

/**
 * @brief Enables the UART device for 8-bit data at
 * the specified {@see BAUDRATE}.
 */
static inline void uart_enable(void) {
  UBRR0H = BAUDRATE >> 8;
  UBRR0L = BAUDRATE;

  // Set RX/TN enabled
  UCSR0B |= _BV(TXEN0) | _BV(RXEN0);

  // Set asynchronous USART
  // Set frame format: 8-bit data, 2-stop bit
  UCSR0C |= _BV(USBS0) | _BV(UCSZ01) | _BV(UCSZ00);
}

/**
 * @brief Transmits a character through the UART device.
 * @param data The data to be sent.
 */
static inline void uart_transmit(unsigned char data) {
  while (!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
}

/**
 * @brief Receives a character from the UART device.
 * @return Received character from the UART.
 */
static inline unsigned char uart_receive(void) {
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

#ifdef __cplusplus
} // extern "C"
#endif
