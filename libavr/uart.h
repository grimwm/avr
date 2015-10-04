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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <util/setbaud.h>

#ifndef BAUD
#  define BAUD 9600
#  warning BAUD rate not set.  Setting BAUD rate to 9600.
#endif // BAUD

// #define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

typedef enum {
  UM_Asynchronous,
  UM_Synchronous,
  UM_MasterSPI,
} UARTMode;

/**
 * @brief Enables the UART device for 8-bit data at
 * the specified {@see BAUDRATE}.
 */
static inline void uart_enable(UARTMode syncMode) {
  // UBRR0H = BAUDRATE >> 8;
  // UBRR0L = BAUDRATE & 0xFF;
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

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
