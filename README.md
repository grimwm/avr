This code supports AVR MCUs utilizing avr-libc by providing libraries additional support libraries for the PWMs and other handy hardware features.  It also has some included code and scripts for helping getting other devices setup that can be used in conjunction with lots of robotic experiments and products, such as the PS3 Dual Shock Controller.  Even the Makefile's "install" target assumes the RPI's GPIO usage at the current time.

It is worth noting that at the moment, items not related directly to the AVR are mostly targeted at Raspberry Pi use.  They will work on other machines, but some of the scripts may need to be modified.  I hope to change that after things solidify more.

Please note that right now, this code is very crude and is currently covered under the GPLv2.0 license (see LICENSE for details).  I will likely put certain portions of it (even some existing code) under the LGPL or BSD license in the future, but not yet.

Usage
-----
    ./configure avr -mcu <mcu_type> -fcpu <hz>
    cd build
    make

    # currently assumes you're using linuxgpio and have set everything up properly in /etc/avrdude.conf
    make install

Example:

    ./configure avr -mcu atmega88 -fcpu 8000000
    cd build
    make all install
