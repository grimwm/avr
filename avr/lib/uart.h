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
 * @param bad_parity If not NULL, stores 1 on parity error; 0 otherwise.
 * @return Received character from the UART.
 */
uint8_t uart0_receive(uint8_t* bad_parity);


#ifdef __cplusplus
} // extern "C"
#endif
