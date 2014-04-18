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

As an output, the program lists all the mp709 devices connected to the box with the pathes
and internal IDs (not persistent, may change after update or usb device reconnections)

You can use path, internal ID or "all" to mark the device(s)

If the device is unresponsive there would be a message in stderr, but the program returns 0 in any case.

EXAMPLES
----------------

    ./mp709 on 1 3 /dev/hidraw5 off /dev/hidraw2 4

    ./mp709 off all on 2

    ./mp709 on 1 off 2 on 3 off 4

    ./mp709 on 2 off all
    pointless: the commandline is parsed from left to right,
    so "on 2" will be ignored

    ./mp709 on 2 off 2
    same as before


