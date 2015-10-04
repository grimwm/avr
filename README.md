AVR Toolkit
===========

This code supports AVR MCUs utilizing avr-libc by providing additional support
libraries for the PWMs and other handy hardware features.  It also has some
included code and scripts for helping getting other devices setup that can
be used in conjunction with lots of robotic experiments and products,
such as the PS3 Dual Shock Controller.

It is worth noting that at the moment, items not related directly to the AVR
are mostly targeted at Raspberry Pi use.  They will work on other machines,
but some of the scripts may need to be modified.  I hope to change that after
things solidify more.

Please note that right now, this code is very crude and is currently covered
under the **GPLv2.0 license** (see *LICENSE* for details).  I will likely put
certain portions of it under the LGPL or BSD license in the future, but not yet.

Build Requirements
------------------
Generally speaking, this project has been tested under Linux.  It is likely
to work under Windows with some finagling, but it hasn't been a focus of this
project at this time.  However, we accept pull requests, and we'd love to
give more people access to this set of tools.

Having said that, assuming you already have your favorite flavor of Linux
installed, you will need these tools:

  1. cmake
  1. gcc-avr
  1. libc-avr
  1. avrdude

Usage
-----
There are two basic modes for building applications.  The first lets you build
programs for the AVR, and the second lets you build applications for the PC.

    ./configure -b <build_dir> avr -mcu <mcu_type> -fcpu <hz>
    OR
    ./configure -b <build_dir> default
    
    cd <build_dir>
    make

The command for installing programs is specific to each target architecture
and will be outlined using examples.

### Example AVR Code:

    # Prepare a build directory for the ATmega88 with a CPU speed of 8 MHz.
    ./configure -b build_avr avr -mcu atmega88 -fcpu 8000000
    cd build_avr

    # Currently, the cmake files assume you're using linuxgpio (e.g. on the
    # Raspberry Pi) and have set everything up properly in /etc/avrdude.conf.
    # See "Raspberry Pi Setup" for some tips on how to configure avrdude
    # and properly connect your AVR directly to a Raspberry Pi.
    #
    # Then, to install the PWM code, for example:
    make install_pwm
    
### Example PC Code:

    ./configure -b build_pc default
    cd build_pc

    # Then, to build all control modules:
    make

Raspberry Pi Setup
------------------
So, you're using a Raspberry Pi (RPi) to communicate with and program your AVR
chips, are you?  Well, congratulations!  It's a fine little setup, and this
section of the notes is specifically to guide you through that setup
process.  This document, however, will only talk about the SPI setup and will
not cover any specifics on how to power your AVR (hint: the RPi has power and
ground pins, and you should connect ALL the AVR ground pins up to a common
ground, btw).  While going through this section, you should refer
to <http://pi.gadgetoid.com/pinout>.

I apologize in advance for not providing any images or diagrams
just yet.  I just want to get this information out to you in any form
possible, even in a raw form, so that you might be able to get started.

Firstly, we're going to assume you want to use the GPIO header to program
your AVR.  You are free to use whatever power source you want, but you have
to make sure your AVR only sends 3.3v signals back to the RPi.  So, if you
power the AVR using something other than the 3.3v (e.g. using the 3.3v
power supply pin on the RPi), please buy a voltage level translator or build
something yourself.  You do not want "the magic smoke".

Anyway, you will be using the SPI pins on your RPi, which are the MOSI, MISO,
and SCK pins.  You also need to use one of the digital output pins as your
~RST signal for the AVR.  You should also connect the RPi's GND to your AVR,
even if the AVR is connected to another power source, because it will create
a common ground.

I'm not exactly an EE expert, but I don't think you should encounter any
ground loops (at least, I didn't, which, extrapolating from a sample size of 1,
no one in the world should have probelems, haha).

Connect the SCK pins on both the RPi and AVR directly together.  Then,
connect the MISO on the RPi to the MOSI on the AVR.  Next, quite obviously,
connect the MOSI on the RPi to the MISO on the AVR.  Finally, connect
whatever pin you designated as the ~RST signal pin to your AVR's ~RST pin.
And by the way, it wouldn't hurt to put a pull-up resistor between the AVR's
~RST pin and Vcc.

Moving on, you're ready to setup `/etc/avrdude.conf` on your RPi to handle
data transfers with the SPI.  On my model B, I chose BCM 22, allowing me
physical pins {15,17,19,21,23,25}, which are located geographically close.
So, assuming you use BCM 22, this is what your `avrdude.conf` would look like
for "linuxgpio":

    programmer
      id    = "linuxgpio";
      desc  = "Use the Linux sysfs interface to bitbang GPIO lines";
      type  = "linuxgpio";
      reset = 22;
      sck   = 11;
      mosi  = 10;
      miso  = 9;
    ;
