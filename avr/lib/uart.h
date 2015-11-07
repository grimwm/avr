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

typedef enum {
  UM_Asynchronous,
  UM_Synchronous,
  UM_MasterSPI,
} UARTMode;

/**
 * @brief Enables the UART device for 8-bit data at
 * the specified {@see BAUD}.
 */
void uart0_enable(UARTMode syncMode);

/**
 * @brief Setup stdout to "print" through the serial port,
 * making functions like printf(3) go through serial.
 */
void uart0_setup_stdout(void);

/**
 * @brief Transmits a character through the UART device.
 * @param data The data to be sent.
 */
void uart0_transmit(uint8_t data);

/**
 * @brief Write a series of data to the UART device.
 * @param data
 * @param length
 */
void uart0_write(uint8_t* data, uint8_t length);

/**
 * @brief Determine whether or not the UART has data for reception.
 * @return 1 if the the UART device has data, 0 otherwise.
 */
uint8_t uart0_receive_buffer_full(void);

/**
 * @brief Receives a character from the UART device.
 * @return Received character from the UART.
 */
uint8_t uart0_receive(void);


#ifdef __cplusplus
} // extern "C"
#endif
