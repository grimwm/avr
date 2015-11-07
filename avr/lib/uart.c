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

#include "math.h"

#include <avr/io.h>
#include <stdio.h>

/*
 * The math for UBBR_1X and UBBR_2X may look a bit odd, but the purpose of is
 * to allow UBBR to get the correct value by "doubling" the addition of
 * 0.5 (e.g. 1.0) by delaying when we multiply a 1/2 out of the equation.
 * This way, when we divide by 2, we get rounded to the nearest integer.
 */
#define UBBR_1X ((F_CPU / 8 / BAUD - 1) / 2)
#define UBBR_2X ((F_CPU / 4 / BAUD - 1) / 2)
#define UBBR_1X_ERROR_RATE ABS(0xFF - ((0xFF * F_CPU / (16 * (UBBR_1X + 1))) / BAUD))
#define UBBR_2X_ERROR_RATE ABS(0xFF - ((0xFF * F_CPU / ( 8 * (UBBR_2X + 1))) / BAUD))

/*
 * Choose best settings for either the least amount of error or U2X if it's the
 * only mode that will work with our clock rate.
 */
#if BAUD > F_CPU / 16 || UBBR_2X_ERROR_RATE < UBBR_1X_ERROR_RATE
#define BAUDBITS UBBR_2X
#define SET_U2X0() UCSR0A |= _BV(U2X0)
#else
#define BAUDBITS UBBR_1X
#define SET_U2X0() UCSR0A &= ~_BV(U2X0)
#endif

#if AVR_UART_ISR_RX_ENABLE || AVR_UART_ISR_TX_ENABLE
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE        64
#define UART_BUFFER_OFFSET(i)   ((UART_BUFFER_SIZE-1) & (i))
typedef struct {
  uint8_t buffer[UART_BUFFER_SIZE];
  uint8_t head, tail;
  uint8_t nitems;
} RingBuffer;
#endif // AVR_UART_ISR_RX_ENABLE || AVR_UART_ISR_TX_ENABLE

#ifdef AVR_UART_ISR_RX_ENABLE
static RingBuffer g_rx_buffer;
#endif

#ifdef AVR_UART_ISR_TX_ENABLE
static RingBuffer g_tx_buffer;
#endif

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
  static uint16_t baudrate = ((F_CPU / 16 / BAUD) - 1);
  UBRR0H = baudrate >> 8;
  UBRR0L = baudrate;

  (void)UDR0; // clear any data currently in the buffer

  SET_U2X0();

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

#ifdef AVR_UART_ISR_RX_ENABLE
ISR(USART_RX_vect) {
  uint8_t data = UDR0; /* retrieve data and clear status */

  if (UART_BUFFER_SIZE == g_rx_buffer.nitems) {
    return;
  }

  g_rx_buffer.buffer[g_rx_buffer.tail] = data;
  g_rx_buffer.tail = UART_BUFFER_OFFSET(g_rx_buffer.tail + 1);
  ++g_rx_buffer.nitems;
}

uint8_t uart0_receive(void) {
  while (0 == g_rx_buffer.nitems); /* no data available */

  uint8_t result = g_rx_buffer.head;
  g_rx_buffer.head = UART_BUFFER_OFFSET(g_rx_buffer.head + 1);
  --g_rx_buffer.nitems;
  return result;
}
#else
uint8_t uart0_receive(void) {
  while (0 == (UCSR0A & _BV(RXC0)));
  return UDR0;
}
#endif // AVR_UART_ISR_RX_ENABLE

#ifdef AVR_UART_ISR_TX_ENABLE
ISR(USART_UDRE_vect) {
  if (0 == g_tx_buffer.nitems) {
    UCSR0B &= ~_BV(UDRIE0); /* clear interrupt flag */
  } else {
    UDR0 = g_tx_buffer.buffer[g_tx_buffer.head];
    g_tx_buffer.head = UART_BUFFER_OFFSET(g_tx_buffer.head + 1);
    --g_tx_buffer.nitems;
  }
}

void uart0_transmit(uint8_t data) {
  while (UART_BUFFER_SIZE == g_tx_buffer.nitems); /* buffer full */

  g_tx_buffer.tail = UART_BUFFER_OFFSET(g_tx_buffer.tail + 1);
  g_tx_buffer.buffer[g_tx_buffer.tail] = data;
  ++g_tx_buffer.nitems;
}
#else
void uart0_transmit(uint8_t data) {
  while (0 == (UCSR0A & _BV(UDRE0)));
  UDR0 = data;
}
#endif // AVR_UART_ISR_TX_ENABLE

void uart0_write(uint8_t* data, uint8_t length) {
  for (uint8_t i = 0; i < length; ++i) {
    uart0_transmit(data[i]);
  }
}
