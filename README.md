mp709
=====

Console client for mp709 USB-relay

BUILD REQUIREMENTS
------------------

debian/ubuntu:
libhidapi-dev

fedora/openSUSE:
hidapi-devel

RUNTIME REQUIREMENTS
--------------------

To run the program as non-root add udev rule like this:

    ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="05df", MODE="0660", GROUP="masterkit"

Add user who works with mp709 to group "masterkit"

USAGE
-------------------

As an output, the program lists all the mp709 devices connected to the box with the pathes,
internal IDs (not persistent, may change after software update or usb device reconnections), and unique
hardware IDs (always the same for the same device)

You can issue **on** or **off** commands to the individual relays by using the path,
internal IDs (decimal integer) or hardware IDs (hex integer), or you can use **all** to address all the devices.

If the device is unresponsive there would be a message in stderr, but the program returns 0 in any case.

The non-existent devices would be ignored without errors.

EXAMPLES
----------------

    ./mp709 on 1 3 /dev/hidraw5 off /dev/hidraw2 4
Switches on the devices with internal IDs 1 and 3 and the path /dev/hidraw5, and off the device with internal ID 4 and the path /dev/hidraw2.

    ./mp709 off all on 2 0x17F
Switches off all the devices except the ones with internal ID 2 and a hardware ID 0x17F, which are switched on instead.

    ./mp709 on 1 off 2 on 3 off 4
Switches on the devices with internal IDs 1 and 3, and off the devices with internal IDs 2 and 4.

    ./mp709 on 2 off all
Switches off all the devices. The program parses the input from left to the right, so "on 2" will be ignored.
If the device with internal ID 2 was off, it will remain so, without switching on and off.

    ./mp709 on 2 off 2
Switches off the device with internal ID 2. If it was off, it will remain so, without switching on and off.

    ./mp709 on all
    sleep 5
    ./mp709 off all
Hardware reboot script for the devices, connected to the NC contacts of the relays.
