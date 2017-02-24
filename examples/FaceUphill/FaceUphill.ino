// This demo uses the Romi 32U4's accelerometer to detect whether
// it is on a slanted surface.  If it is on a slanted surface,
// then it uses the motors to face uphill.
//
// It also uses the encoders to avoid rolling down the surface.
//
// Please note that this example is not very robust and it might
// be hard to modify it to behave differently.  The accelerometer
// readings are affected by the movement of the Romi, so if you
// change the code for controlling the motors, you might also affect
// the accelerometer readings.
//
// Also, if the robot is pointing directly downhill, it might not
// move, because the y component of the acceleration would be
// close to 0.
//
// To run this sketch, you will need to install the LSM6 library:
//
// https://github.com/pololu/lsm6-arduino

#include <Wire.h>
#include <Romi32U4.h>
#include <LSM6.h>

const int16_t maxSpeed = 150;

Romi32U4Motors motors;
Romi32U4LCD lcd;
Romi32U4ButtonA buttonA;
Romi32U4Encoders encoders;
LSM6 imu;

void setup()
{
  // Start I2C and initialize the LSM6 accelerometer/gyro.
  Wire.begin();
  imu.init();
  imu.enableDefault();

  // Set the gyro full scale to 1000 dps because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL2_G, 0b10001000);

  // Set the accelerometer full scale to 16 g because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL1_XL, 0b10000100);

  lcd.clear();
  lcd.print(F("Press A"));
  buttonA.waitForPress();
  lcd.clear();
}

void loop()
{
  // Read the acceleration from the LSM6DS33.
  // A value of 2048 corresponds to approximately 1 g.
  imu.read();
  int16_t x = imu.a.x;
  int16_t y = imu.a.y;
  int32_t magnitudeSquared = (int32_t)x * x + (int32_t)y * y;

  // Display the X and Y acceleration values on the LCD
  // every 150 ms.
  static uint8_t lastDisplayTime;
  if ((uint8_t)(millis() - lastDisplayTime) > 150)
  {
    lastDisplayTime = millis();
    lcd.gotoXY(0, 0);
    lcd.print(x);
    lcd.print(F("       "));
    lcd.gotoXY(0, 1);
    lcd.print(y);
    lcd.print(F("       "));
  }

  // Use the encoders to see how much we should drive forward.
  // If the robot rolls downhill, the encoder counts will become
  // negative, resulting in a positive forwardSpeed to counteract
  // the rolling.
  int16_t forwardSpeed = -(encoders.getCountsLeft() + encoders.getCountsRight());
  forwardSpeed = constrain(forwardSpeed, -maxSpeed, maxSpeed);

  // See if we are actually on an incline.
  // 2048 * sin(5 deg) = 178
  int16_t turnSpeed;
  if (magnitudeSquared > (int32_t)178 * 178)
  {
    // We are on an incline of more than 5 degrees, so
    // try to face uphill using a feedback algorithm.
    turnSpeed = y / 2;
    ledYellow(1);
  }
  else
  {
    // We not on a noticeable incline, so don't turn.
    turnSpeed = 0;
    ledYellow(0);
  }

  // To face uphill, we need to turn so that the X acceleration
  // is negative and the Y acceleration is 0.  Therefore, when
  // the Y acceleration is positive, we want to turn to the
  // left (counter-clockwise).
  int16_t leftSpeed = forwardSpeed - turnSpeed;
  int16_t rightSpeed = forwardSpeed + turnSpeed;

  // Constrain the speeds to be between -maxSpeed and maxSpeed.
  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

  motors.setSpeeds(leftSpeed, rightSpeed);
}
