// This example reads the raw values from the LSM6DS33
// accelerometer and gyro and and the LIS3MDL magnetometer on the
// Balboa 32U4, and prints those raw values to the serial
// monitor.
//
// The accelerometer readings can be converted to units of g
// using the conversion factors specified in the "Mechanical
// characteristics" table in the LSM6DS33 datasheet.  We use a
// full scale (FS) setting of +/- 16 g, so the conversion factor
// is 0.488 mg/LSB (least-significant bit).  A raw reading of
// 2048 would correspond to 1 g.
//
// The gyro readings can be converted to degrees per second (dps)
// using the "Mechanical characteristics" table in the LSM6DS33
// datasheet.  We use a full scale (FS) of +/- 1000 dps so the
// conversion factor is 35 mdps/LSB.  A raw reading of 2571
// would correspond to 90 dps.
//
// The magnetometer readings are more difficult to interpret and
// will usually require calibration.
//
// To run this sketch, you will need to install the LSM6 and
// LIS3MDL libraries:
//
// https://github.com/pololu/lsm6-arduino
// https://github.com/pololu/lis3mdl-arduino

#include <Wire.h>
#include <Balboa32U4.h>
#include <LSM6.h>
#include <LIS3MDL.h>

LSM6 imu;
LIS3MDL mag;

char report[120];

void setup()
{
  Wire.begin();

  if (!mag.init())
  {
    // Failed to detect the LIS3MDL.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect the LIS3MDL."));
      delay(100);
    }
  }

  mag.enableDefault();

  if (!imu.init())
  {
    // Failed to detect the LSM6.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect the LSM6."));
      delay(100);
    }
  }

  imu.enableDefault();

  // Set the gyro full scale to 1000 dps because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL2_G, 0b10001000);

  // Set the accelerometer full scale to 16 g because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL1_XL, 0b10000100);
}

void loop()
{
  imu.read();
  mag.read();

  snprintf_P(report, sizeof(report),
    PSTR("A: %6d %6d %6d    M: %6d %6d %6d    G: %6d %6d %6d"),
    imu.a.x, imu.a.y, imu.a.z,
    mag.m.x, mag.m.y, mag.m.z,
    imu.g.x, imu.g.y, imu.g.z);
  Serial.println(report);

  delay(100);
}
