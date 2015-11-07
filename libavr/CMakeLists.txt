# Non-interrupt version of the UART.
add_library(io-noint STATIC uart.c)

# Interrupt-driven version of the UART.
add_library(io STATIC uart.c)
set_property(TARGET io APPEND PROPERTY
  COMPILE_DEFINITIONS AVR_UART_ISR_RX_ENABLE AVR_UART_ISR_TX_ENABLE)
