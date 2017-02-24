// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

/*! \file Romi32U4Motors.h */

#pragma once

#include <stdint.h>

/*! \brief Controls motor speed and direction on the Romi 32U4.
 *
 * This library uses Timer 1, so it will conflict with any other libraries using
 * that timer. */
class Romi32U4Motors
{
  public:

    /** \brief Flips the direction of the left motor.
     *
     * You can call this function with an argument of \c true if the left motor
     * of your Romi was not wired in the standard way and you want a
     * positive speed argument to correspond to forward movement.
     *
     * \param flip If true, then positive motor speeds will correspond to the
     * direction pin being high.  If false, then positive motor speeds will
     * correspond to the direction pin being low.
     */
    static void flipLeftMotor(bool flip);

    /** \brief Flips the direction of the right motor.
     *
     * You can call this function with an argument of \c true if the right motor
     * of your Romi was not wired in the standard way and you want a
     * positive speed argument to correspond to forward movement.
     *
     * \param flip If true, then positive motor speeds will correspond to the
     * direction pin being high.  If false, then positive motor speeds will
     * correspond to the direction pin being low. */
    static void flipRightMotor(bool flip);

    /** \brief Sets the speed for the left motor.
     *
     * \param speed A number from -300 to 300 representing the speed and
     * direction of the left motor.  Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
    static void setLeftSpeed(int16_t speed);

    /** \brief Sets the speed for the left motor.
     *
     * \param speed A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
    static void setRightSpeed(int16_t speed);

    /** \brief Sets the speed for the left motor.
     *
     * \param leftSpeed A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward.
     * \param rightSpeed A number from -300 to 300 representing the speed and
     * direction of the right motor. Values of -300 or less result in full speed
     * reverse, and values of 300 or more result in full speed forward. */
    static void setSpeeds(int16_t leftSpeed, int16_t rightSpeed);

    /** \brief Turns turbo mode on or off.
     *
     * By default turbo mode is off.  When turbo mode is on, the range of speeds
     * accepted by the other functions in this library becomes -400 to 400
     * (instead of -300 to 300).  Turning turbo mode on allows the Romi to move
     * faster but could decrease the lifetime of the motors.
     *
     * This function does not have any immediate effect on the speed of the
     * motors; it just changes the behavior of the other functions in this
     * library.
     *
     * \param turbo If true, turns turbo mode on.
     *   If false, turns turbo mode off. */
    static void allowTurbo(bool turbo);

  private:

    static inline void init()
    {
        static bool initialized = false;

        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    static void init2();

    static int16_t maxSpeed;
    static bool flipLeft;
    static bool flipRight;
};
