// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/** \file Romi32U4Buttons.h **/

#pragma once

#include <Pushbutton.h>
#include <FastGPIO.h>
#include <USBPause.h>
#include <util/delay.h>

/*! The pin number for the pin connected to button A on the Romi 32U4. */
#define ROMI_32U4_BUTTON_A 14

/*! The pin number for the pin connected to button B on the Romi 32U4.  Note
 * that this is not an official Arduino pin number so it cannot be used with
 * functions like digitalRead, but it can be used with the FastGPIO library. */
#define ROMI_32U4_BUTTON_B IO_D5

/*! The pin number for the pin conencted to button C on the Romi 32U4. */
#define ROMI_32U4_BUTTON_C 17

/*! \brief Interfaces with button A on the Romi 32U4. */
class Romi32U4ButtonA : public Pushbutton
{
public:
    Romi32U4ButtonA() : Pushbutton(ROMI_32U4_BUTTON_A)
    {
    }
};

/*! \brief Interfaces with button B on the Romi 32U4.
 *
 * The pin used for button B is also used for the TX LED.
 *
 * This class temporarily disables USB interrupts because the Arduino core code
 * has USB interrupts enabled that sometimes write to the pin this button is on.
 *
 * This class temporarily sets the pin to be an input without a pull-up
 * resistor.  The pull-up resistor is not needed because of the resistors on the
 * board. */
class Romi32U4ButtonB : public PushbuttonBase
{
public:

    virtual bool isPressed()
    {
        USBPause usbPause;
        FastGPIO::PinLoan<ROMI_32U4_BUTTON_B> loan;
        FastGPIO::Pin<ROMI_32U4_BUTTON_B>::setInputPulledUp();
        _delay_us(3);
        return !FastGPIO::Pin<ROMI_32U4_BUTTON_B>::isInputHigh();
    }
};

/*! \brief Interfaces with button C on the Romi 32U4.
 *
 * The pin used for button C is also used for the RX LED.
 *
 * This class temporarily disables USB interrupts because the Arduino core code
 * has USB interrupts enabled that sometimes write to the pin this button is on.
 *
 * This class temporarily sets the pin to be an input without a pull-up
 * resistor.  The pull-up resistor is not needed because of the resistors on the
 * board. */
class Romi32U4ButtonC : public PushbuttonBase
{
public:

    virtual bool isPressed()
    {
        USBPause usbPause;
        FastGPIO::PinLoan<ROMI_32U4_BUTTON_C> loan;
        FastGPIO::Pin<ROMI_32U4_BUTTON_C>::setInputPulledUp();
        _delay_us(3);
        return !FastGPIO::Pin<ROMI_32U4_BUTTON_C>::isInputHigh();
    }
};

