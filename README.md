# XBow 400 Library

This library is for interfacing with a XBow IMU400CC series IMU.

# Dependencies

* cmake (http://www.cmake.org/): For building stuff.
* serial (https://github.com/wjwwood/serial): For interfacing to a serial port.

# Building

You can use the given Makefile:

    make

OR you can build using cmake:

    mkdir build && cd build
    cmake .. # or cmake-gui .. for a graphical interface
    make

# Running the example:

    bin/xbow400_example

# Using this library:

Refer to the `src/xbow400_example.cc` and the `include/xbow400/xbow400.h` for information about how to use the library.

