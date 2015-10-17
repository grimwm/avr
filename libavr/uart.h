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
void uart0_transmit(unsigned char data);

/**
 * @brief Receives a character from the UART device.
 * @return Received character from the UART.
 */
unsigned char uart0_receive(void);


#ifdef __cplusplus
} // extern "C"
#endif
