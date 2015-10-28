/**
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.  If a copy of the MPL
 * was not distributed with this file, you can obtain one at
 * https://mozilla.org/MPL/2.0/.
 *
 * Copyright William Grim, 2015
 */

#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include <stdint.h>
#include <string.h>

#include "uart.h"

static void (*startapp)(void) = 0x0000;

#define TIMEOUT_LOOPS ((F_CPU/1000)*BOOT_TIMEOUT_MS)

#if (SPM_PAGESIZE-1) & SPM_PAGESIZE
#  error SPM_PAGESIZE must be a power of 2
#else
#  define PAGE_ADDR_BASE(addr) ((addr) & ~(SPM_PAGESIZE-1))
#  define PAGE_OFFSET(addr) ((addr) & (SPM_PAGESIZE-1))
#endif

/**
 * https://en.wikipedia.org/wiki/Intel_HEX#Record_structure
 */
typedef struct {
  uint8_t   length;
  uint16_t  address;
} IntelHexRecordHeader;

static uint8_t g_page[SPM_PAGESIZE];

static void flash_write_page(uint16_t page_addr) {
  /*
   * Per datasheets, we have to erase a page at addr
   * and then write at the same addr.
   */
  boot_page_erase_safe(page_addr);

  /* Copy RAM to SPM page buffer */
  for (uint8_t i = 0; i < SPM_PAGESIZE; i += 2) {
    uint16_t word = g_page[i] | g_page[i+1] << 8;
    boot_page_fill_safe(page_addr+i, word);
  }

  /* Write our page buffer to flash. */
  boot_page_write_safe(page_addr);
}

int main(void) __attribute__((OS_main)) __attribute__((section(".init9")));
int main(void) {
  uint8_t sreg = SREG;
  cli();

  uart0_enable(UM_Asynchronous);

  DDRB = 0xFF;

  PORTB = 0xFF;
  _delay_ms(100);
  PORTB = 0x00;
  _delay_ms(100);
  PORTB = 0xFF;
  _delay_ms(100);
  PORTB = 0x00;

  SREG = 0;     /* status register disabled */
  SP = RAMEND;  /* stack pointer at RAMEND */

  //uart0_write((uint8_t*)"AVRR", 4); /* send readiness header */
  uint8_t crc;
  IntelHexRecordHeader ihex = { 0x00, 0x0000 };
  uint16_t page_base_addr = ~0;

restart:
  crc = 0;
  for (uint32_t timeout = 0; timeout < TIMEOUT_LOOPS; ++timeout) {
    if (!uart0_receive_buffer_full()) {
      continue;
    }

    uint8_t command = uart0_receive();

    switch (command) {
    case 'A': /* Set ihex address. */
      ihex.address = uart0_receive() << 8;
      ihex.address |= uart0_receive();

      crc += ihex.address >> 8;
      uart0_transmit(ihex.address >> 8);

      crc += ihex.address & 0xFF;
      uart0_transmit(ihex.address & 0xFF);

      /* timeout = 0; */
      break;
    case 'L': /* Set data length. */
      crc += ihex.length = uart0_receive();
      uart0_transmit(ihex.length);
      /* timeout = 0; */
      break;
    case 'D': /* Write data.  Return CRC. */ {
      uint16_t addr = ihex.address;
      page_base_addr = PAGE_ADDR_BASE(addr);
      for (uint8_t i = 0; i < ihex.length; ++i, ++addr) {
        if (PAGE_ADDR_BASE(addr) != page_base_addr) {
          flash_write_page(page_base_addr);
          page_base_addr = PAGE_ADDR_BASE(addr);
        }

        crc += g_page[PAGE_OFFSET(addr)] = uart0_receive();
        uart0_transmit(g_page[PAGE_OFFSET(addr)]);
      }
      flash_write_page(page_base_addr);

      uart0_transmit(~crc + 1);
      crc = 0;
      /* timeout = 0; */
      break;
    }
    case 'E': /* End upload; start program. */
      goto startapp;
    default:
      uart0_transmit('?');
      goto restart;
    }
  }

startapp:
  SREG = sreg;
  SP = RAMEND;

  /* enable the RWW section so that we can jump to it after bootloading. */
  boot_rww_enable_safe();

  /* Jump to the application. */
  startapp();

  return 0;
}
