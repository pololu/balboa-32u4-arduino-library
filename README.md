# Balboa32U4 library

Version: 1.1.1<br/>
Release date: 2019-04-05<br/>
[![Build Status](https://travis-ci.org/pololu/balboa-32u4-arduino-library.svg?branch=master)](https://travis-ci.org/pololu/balboa-32u4-arduino-library)<br/>
[www.pololu.com](https://www.pololu.com/)

## Summary

This is a C++ library for the Arduino IDE that helps access the on-board hardware of the Balboa 32U4 control board, which is part of the [Balboa 32U4 balancing robot](https://www.pololu.com/category/210/balboa-robot-and-acessories).  The board is based on the Arduino-compatible ATmega32U4 MCU and integrates motor drivers, encoders, buzzer, buttons, an LSM6DS33 accelerometer and gyro, and an LIS3MDL compass.

The library also makes it easier to interface with the optional [5-Channel reflectance sensor array](https://www.pololu.com/product/3577) that you can add to the Balboa 32U4.

This library does not include code for accessing the LSM6DS33 or LIS3MDL.  If you want to access those sensors, you should install the separate [LSM6](https://github.com/pololu/lsm6-arduino) and [LIS3MDL](https://github.com/pololu/lis3mdl-arduino) libraries.

This library is very similar to the [Romi32U4](https://github.com/pololu/romi-32u4-arduino-library) library.

## Installing the library

If you are using version 1.6.2 or later of the [Arduino software (IDE)](http://www.arduino.cc/en/Main/Software), you can use the Library Manager to install this library:

1. In the Arduino IDE, open the "Sketch" menu, select "Include Library", then "Manage Libraries...".
2. Search for "Balboa32U4".
3. Click the Balboa32U4 entry in the list.
4. Click "Install".

If this does not work, you can manually install the library:

1. Download the [latest release archive from GitHub](https://github.com/pololu/balboa-32u4-arduino-library) and decompress it.
2. Rename the folder "balboa-32u4-arduino-library-master" to "Balboa32U4".
3. Move the "Balboa32U4" folder into the "libraries" directory inside your Arduino sketchbook directory.  You can view your sketchbook location by opening the "File" menu and selecting "Preferences" in the Arduino IDE.  If there is not already a "libraries" folder in that location, you should make the folder yourself.
4. After installing the library, restart the Arduino IDE.

## Examples

Several example sketches are available that show how to use the library.  You can access them from the Arduino IDE by opening the "File" menu, selecting "Examples", and then selecting "Balboa32U4".  If you cannot find these examples, the library was probably installed incorrectly and you should retry the installation instructions above.

## Classes and functions

The main classes and functions provided by the library are listed below:

* Balboa32U4ButtonA
* Balboa32U4ButtonB
* Balboa32U4ButtonC
* Balboa32U4Buzzer
* Balboa32U4Encoders
* Balboa32U4LCD
* Balboa32U4LineSensors
* Balboa32U4Motors
* ledRed()
* ledGreen()
* ledYellow()
* usbPowerPresent()
* readBatteryMillivolts()

## Component libraries

This library also includes copies of several other Arduino libraries inside it which are used to help implement the classes and functions above.

* [FastGPIO](https://github.com/pololu/fastgpio-arduino)
* [PololuBuzzer](https://github.com/pololu/pololu-buzzer-arduino)
* [PololuHD44780](https://github.com/pololu/pololu-hd44780-arduino)
* [Pushbutton](https://github.com/pololu/pushbutton-arduino)
* [QTRSensors](https://github.com/pololu/qtr-sensors-arduino)
* [USBPause](https://github.com/pololu/usb-pause-arduino)

You can use these libraries in your sketch automatically without any extra installation steps and without needing to add any extra `#include` lines to your sketch.

You should avoid adding extra `#include` lines such as `#include <Pushbutton.h>` because then the Arduino IDE might try to use the standalone Pushbutton library (if you previously installed it), and it would conflict with the copy of the Pushbutton code included in this library.  The only `#include` line needed to access all features of this library are:

~~~{.cpp}
#include <Balboa32U4.h>
~~~

## Documentation

For complete documentation, see https://pololu.github.io/balboa-32u4-arduino-library.  If you are already on that page, then click on the links in the "Classes and functions" section above.

## Version history

* 1.1.1 (2019-04-05): Fixed narrowing conversion warning/error in Demo.ino.
* 1.1.0 (2019-04-04): Added Balboa32U4LineSensors class and LineSensorTest example. Improved Balancer example to measure lying down angle and more effectively decide whether it is balancing.
* 1.0.1 (2017-07-17): Fixed a bug that caused errors from the right encoder to get reported as errors from the left encoder.
* 1.0.0 (2017-03-08): Original release.
