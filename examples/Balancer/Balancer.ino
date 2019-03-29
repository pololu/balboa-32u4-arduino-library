// This example shows how to make a Balboa balance on its two
// wheels and drive around while balancing.
//
// To run this demo, you will need to install the LSM6 library:
//
// https://github.com/pololu/lsm6-arduino
//
// To use this demo, place the robot on the ground with the
// circuit board facing up, and then turn it on.  Be careful to
// not move the robot for a few seconds after powering it on,
// because that is when the gyro is calibrated.  During the gyro
// calibration, the red LED is lit.  After the red LED turns off,
// turn the robot so that it is standing up.  It will detect that
// you have turned it and start balancing.
//
// Alternatively, you can press the A button while the robot is
// lying down and it will try to use its motors to kick up into
// the balancing position.
//
// This demo is tuned for the 50:1 high-power gearmotor with
// carbon brushes, 45:21 plastic gears, and 80mm wheels; you will
// need to adjust the parameters in Balance.h for your robot.
//
// After you have gotten the robot balance well, you can
// uncomment some lines in loop() to make it drive around and
// play a song.

#include <Balboa32U4.h>
#include <Wire.h>
#include <LSM6.h>
#include "Balance.h"

LSM6 imu;
Balboa32U4Motors motors;
Balboa32U4Encoders encoders;
Balboa32U4Buzzer buzzer;
Balboa32U4ButtonA buttonA;
Balboa32U4ButtonB buttonB;
Balboa32U4ButtonC buttonC;

void setup()
{
  // Uncomment these lines if your motors are reversed.
  // motors.flipLeftMotor(true);
  // motors.flipRightMotor(true);

  ledYellow(0);
  ledRed(1);
  balanceSetup();
  ledRed(0);
}

const char song[] PROGMEM =
  "!O6 T240"
  "l32ab-b>cl8r br b-bb-a a-r gr g-4 g4"
  "a-r gr g-gg-f er e-r d4 e-4"
  "gr msd8d8ml d-4d4"
  "l32efg-gl8r msd8d8ml d-4d4"
  "<bcd-d e-efg- ga-ab- a4 gr";

void playSong()
{
  if (!buzzer.isPlaying())
  {
    buzzer.playFromProgramSpace(song);
  }
}

void driveAround()
{
  uint16_t time = millis() % 8192;
  uint16_t leftSpeed, rightSpeed;
  if (time < 1900)
  {
    leftSpeed = 20;
    rightSpeed = 20;
  }
  else if (time < 4096)
  {
    leftSpeed = 25;
    rightSpeed = 15;
  }
  else if (time < 4096 + 1900)
  {
    leftSpeed = 20;
    rightSpeed = 20;
  }
  else
  {
    leftSpeed = 15;
    rightSpeed = 25;
  }

  balanceDrive(leftSpeed, rightSpeed);
}

void standUp()
{
  motors.setSpeeds(0, 0);
  buzzer.play("!>grms>g16>g16>g2");
  ledGreen(1);
  ledRed(1);
  ledYellow(1);
  while (buzzer.isPlaying());
  motors.setSpeeds(-MOTOR_SPEED_LIMIT, -MOTOR_SPEED_LIMIT);
  delay(400);
  motors.setSpeeds(150, 150);
  for (uint8_t i = 0; i < 20; i++)
  {
    delay(UPDATE_TIME_MS);
    balanceUpdateSensors();
    if(angle < 60000)
    {
      break;
    }
  }
  motorSpeed = 150;
  balanceResetEncoders();
}

void loop()
{
  static bool enableSong = false;
  static bool enableDrive = false;

  balanceUpdate();

  if (isBalancing())
  {
    if (enableSong)   { playSong(); }
    if (enableDrive)  { driveAround(); }
  }
  else
  {
    buzzer.stopPlaying();
    balanceDrive(0, 0); // reset driving speeds

    if (buttonA.getSingleDebouncedPress())
    {
      enableSong = false;
      enableDrive = false;
      standUp();
    }
    else if (buttonB.getSingleDebouncedPress())
    {
      enableSong = false;
      enableDrive = true;
      standUp();
    }
    else if (buttonC.getSingleDebouncedPress())
    {
      enableSong = true;
      enableDrive = true;
      standUp();
    }
  }

  // Illuminate the red LED if the last full update was too slow.
  ledRed(balanceUpdateDelayed());

  // Display feedback on the yellow and green LEDs depending on
  // the variable fallingAngleOffset.  This variable is similar
  // to the risingAngleOffset used in Balance.cpp.
  //
  // When the robot is rising toward vertical (not falling),
  // angleRate and angle have opposite signs, so this variable
  // will just be positive or negative depending on which side of
  // vertical it is on.
  //
  // When the robot is falling, the variable measures how far off
  // it is from a trajectory starting it almost perfectly
  // balanced then falling to one side or the other with the
  // motors off.
  //
  // Since this depends on ANGLE_RATE_RATIO, it is useful for
  // calibration.  If you have changed the wheels or added weight
  // to your robot, you can try checking these items, with the
  // motor power OFF (powered by USB):
  //
  // 1. Try letting the robot fall with the Balboa 32U4 PCB up.
  //    The green LED should remain lit the entire time.  If it
  //    sometimes shows yellow instead of green, reduce
  //    ANGLE_RATE_RATIO.
  //
  // 2. If it is tilted beyond vertical and given a push back to
  //    the PCB-up side again, the yellow LED should remain lit
  //    until it hits the ground.  If you see green, increase
  //    ANGLE_RATE_RATIO.
  //
  // In practice, it is hard to achieve both 1 and 2 perfectly,
  // but if you can get close, your constant will probably be
  // good enough for balancing.
  int32_t fallingAngleOffset = angleRate * ANGLE_RATE_RATIO - angle;
  if (fallingAngleOffset > 0)
  {
    ledYellow(1);
    ledGreen(0);
  }
  else
  {
    ledYellow(0);
    ledGreen(1);
  }
}
