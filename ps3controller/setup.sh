#!/bin/bash

# This script installs and sets up several tools to be able to use the PS3 SixAxis controllers.
# For more information, please see http://booting-rpi.blogspot.ro/2012/08/dualshock-3-and-raspberry-pi.html.
#
# Some more advanced information is available at http://www.pabr.org/sixlinux/sixlinux.en.html.

PACKAGES="bluetooth blueman bluez-utils bluez-compat bluez-hcidump checkinstall libusb-dev libbluetooth-dev joystick"

sudo apt-get update || exit $?
sudo apt-get install -y $PACKAGES || exit $?

if [ ! -f sixpair.c ] ; then
    echo "Fetching sixpair..."
    wget "http://www.pabr.org/sixlinux/sixpair.c" || exit
fi

if [ ! -x sixpair ] ; then
    echo "Building sixpair..."
    gcc -o sixpair sixpair.c -lusb || exit
fi

dpkg -l sixad >/dev/null
if [ $? -ne 0 ] ; then
    if [ ! -f QtSixA-1.5.1-src.tar.gz ] ; then
        echo "Downloading QtSixA: The Sixaxis Joystick Manager..."
        wget http://sourceforge.net/projects/qtsixa/files/QtSixA%201.5.1/QtSixA-1.5.1-src.tar.gz || exit
        tar xfvz QtSixA-1.5.1-src.tar.gz || exit
    fi

    pushd QtSixA-1.5.1/sixad
    echo "Patching sixad so servos work."
    patch bluetooth.cpp ../../bluetooth.patch || exit
    echo "Building QtSixA..."
    make
    sudo mkdir -p /var/lib/sixad/profiles
    sudo checkinstall || exit
    popd
fi

echo "Adding sixad to daemons started at boot..."
sudo update-rc.d sixad defaults || exit

echo -n "Would you like to pair your PS3 controller with this computer (Y/n)? "
read dopair
case $dopair in
    Y|y|"")
        echo "Running sixpair..."
        sudo ./sixpair || exit
        echo -n "Press enter to continue..."
        read
        ;;
esac

echo "Installation complete!"
