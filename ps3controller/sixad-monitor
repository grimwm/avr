#!/bin/bash

JOYSTICK_DEV=/dev/input/js0

while : ; do
    sleep 2

    if [ ! -f $JOYSTICK_DEV ] ; then
        while : ; do
            echo "$JOYSTICK_DEV disappeared!  Restarting bluetoothd and sixad..."
            sudo service sixad stop || continue
            sudo service bluetooth restart || continue
            sudo service sixad start || continue
            break
        done
        echo "bluetoothd and sixad restarted."
    fi
done