MCU=atmega88
F_CPU=8000000

CC=avr-gcc
CFLAGS=-g -Os -std=c99 -Wall -mcall-prologues -mmcu=$(MCU)

OBJ2HEX=avr-objcopy
AVRDUDE=avrdude

FUSE=-U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0x00:m

# TARGET=blinky
TARGET=pwm

.PHONY: all
all:
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) -Ilib -DF_CPU=$(F_CPU)
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex
	rm -f $(TARGET)

.PHONY: install
install: all
	sudo $(AVRDUDE) -p $(MCU) -c linuxgpio -U flash:w:$(TARGET).hex

.PHONY: fuse
fuse:
	sudo $(AVRDUDE) -p $(MCU) -c linuxgpio $(FUSE)

.PHONY: clean
clean:
	rm -f *.hex *.obj *.o
