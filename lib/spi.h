#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

/**
 * Common SPI pins.
 */
#define SCK  PB5
#define MISO PB4
#define MOSI PB3
#define SS   PB2

/**
 * @brief Initialize SPI Master Device.
 *
 * Don't forget to call sei() if you want interrupts.
 *
 * @param use_interrupts Determines whether or not to enable SPI interrupts.
 */
static inline void spi_init_master(int use_interrupts) {
  DDRB |= _BV(MOSI) | _BV(SCK);

  // Enable SPI, Set as Master
  // Prescaler: Fosc/16
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
  if (use_interrupts) {
    SPCR |= _BV(SPIE);
  }
}

/**
 * @brief Initialize SPI Slave Device
 *
 * Don't forget to call sei() if you want interrupts.
 *
 * @param use_interrupts Determines whether or not to enable SPI interrupts.
 */
static inline void spi_init_slave(int use_interrupts) {
  DDRB |= _BV(MISO);

  SPCR = _BV(SPE);
  if (use_interrupts) {
    SPCR |= _BV(SPIE);
  }
}

/**
 * @brief Function to send and receive data for both master and slave
 * @param data The data to send.
 * @return The received data.
 */
static inline unsigned char spi_tranceiver(unsigned char data) {
  SPDR = data;

  // Wait until transmission complete
  while (!(SPSR & _BV(SPIF)));

  return SPDR;
}

/**
 * @brief Send data only.  This method is useful if you're using an interrupt handler
 * to get SPI notifications.
 * @param data The data to send.
 */
static inline void spi_send(unsigned char data) {
  SPDR = data;
}

#ifdef __cplusplus
} // extern "C"
#endif
