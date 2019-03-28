/** \file Balboa32U4LineSensors.h **/

#pragma once

#include <QTRSensors.h>

/** \brief Gets readings from the five line sensors on the reflectance sensor
 * array.
 *
 * Before using the sensors with this class, you must call either
 * setCenterAligned() or setEdgeAligned() depending on the way your sensor
 * array is mounted on the Balboa. See the [product page for the sensor
 * array](https://www.pololu.com/product/3577) for an explanation and pictures
 * of the two options.
 *
 * If the alignment is set appropriately with one of the above functions, the
 * readLineBlack() and readLineWhite() methods will always return values that
 * increase from left to right, with 0 corresponding to the leftmost sensor and
 * 4000 corresponding to the rightmost sensor. (Note that “forward” refers to
 * the rotation direction that would cause a balancing Balboa to move in the
 * direction its battery cover is facing, and left and right are defined
 * accordingly.)
 *
 * See the [Usage Notes in the QTRSensors
 * documentation](https://pololu.github.io/qtr-sensors-arduino/md_usage.html)
 * for an overview of how the methods inherited from the QTRSensors library can
 * be used and some example code.
 */
class Balboa32U4LineSensors : public QTRSensors
{
public:

    Balboa32U4LineSensors()
    {
        setTypeRC();
    }

    /** \brief Configures this object to use a center-aligned sensor array.
     *
     * This function configures this object to interface with a reflectance
     * sensor array using the center-aligned mounting option. In this
     * configuration, S5 is on the left side of the Balboa, S1 is on the right,
     * and CTRL is connected to pin 12. */
    void setCenterAligned()
    {
        setSensorPins((const uint8_t[]){ 5, A4, A3, A2, A0 }, 5);
        setEmitterPin(12);
    }

    /** \brief Configures this object to use a center-aligned sensor array.
     *
     * This function configures this object to interface with a reflectance
     * sensor array using the center-aligned mounting option. In this
     * configuration, S1 is on the left side of the Balboa, S5 is on the right,
     * and CTRL is connected to pin 5. */
    void setEdgeAligned()
    {
        setSensorPins((const uint8_t[]){ A4, A3, A2, A0, 12 }, 5);
        setEmitterPin(5);
    }
};

