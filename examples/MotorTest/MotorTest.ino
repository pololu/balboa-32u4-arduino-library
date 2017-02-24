// This example drives each motor on the Romi forward, then
// backward.  The yellow user LED is on when a motor should be
// running forward and off when a motor should be running
// backward.

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4ButtonA buttonA;

void setup()
{
  // Wait for the user to press button A.
  buttonA.waitForButton();

  // Delay so that the robot does not move away while the user is
  // still touching it.
  delay(1000);
}

void loop()
{
  // Run left motor forward.
  ledYellow(1);
  for (int speed = 0; speed <= 400; speed++)
  {
    motors.setLeftSpeed(speed);
    delay(2);
  }
  for (int speed = 400; speed >= 0; speed--)
  {
    motors.setLeftSpeed(speed);
    delay(2);
  }

  // Run left motor backward.
  ledYellow(0);
  for (int speed = 0; speed >= -400; speed--)
  {
    motors.setLeftSpeed(speed);
    delay(2);
  }
  for (int speed = -400; speed <= 0; speed++)
  {
    motors.setLeftSpeed(speed);
    delay(2);
  }

  // Run right motor forward.
  ledYellow(1);
  for (int speed = 0; speed <= 400; speed++)
  {
    motors.setRightSpeed(speed);
    delay(2);
  }
  for (int speed = 400; speed >= 0; speed--)
  {
    motors.setRightSpeed(speed);
    delay(2);
  }

  // Run right motor backward.
  ledYellow(0);
  for (int speed = 0; speed >= -400; speed--)
  {
    motors.setRightSpeed(speed);
    delay(2);
  }
  for (int speed = -400; speed <= 0; speed++)
  {
    motors.setRightSpeed(speed);
    delay(2);
  }

  delay(500);
}
