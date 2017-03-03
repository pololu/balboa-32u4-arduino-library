// This example shows how to make a Balboa balance on its two
// wheels and drive around while balancing.
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

void setup()
{
  // Uncomment these lines if your motors are reversed.
  // motors.flipLeftMotor(true);
  // motors.flipRightMotor(true);

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
  "cd-de-efg-g l32fg-ga-l8r gr g-4 gr";

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
  uint16_t left, right;
  if (time < 1900)
  {
    left = 20;
    right = 20;
  }
  else if (time < 4096)
  {
    left = 25;
    right = 15;
  }
  else if (time < 4096 + 1900)
  {
    left = 20;
    right = 20;
  }
  else
  {
    left = 15;
    right = 25;
  }

  balanceDrive(left, right);
}

void standUp()
{
  motors.setSpeeds(0,0);
  buzzer.play("!frfr");
  while (buzzer.isPlaying());
  buzzer.play(">c2");
  motors.setSpeeds(-MOTOR_SPEED_LIMIT, -MOTOR_SPEED_LIMIT);
  delay(400);
  motors.setSpeeds(150,150);
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
  balanceUpdate();

  if (isBalancing())
  {
    // Once you have it balancing well, uncomment these lines for
    // something fun.

    // playSong();
    // driveAround();
  }
  else
  {
    buzzer.stopPlaying();

    if (buttonA.getSingleDebouncedPress())
    {
      standUp();
    }
  }

  // Illuminate the red LED if the last full update was too slow.
  ledRed(balanceUpdateDelayed());

  // Display feedback on the yellow and green LEDs.  This is
  // useful for calibrating ANGLE_RATE_RATIO: if the robot is
  // released from nearly vertical and falls onto its bottom, the
  // green LED should remain lit the entire time.  If it is
  // tilted beyond vertical and given a push to fall back to its
  // bottom side, the yellow LED should remain lit until it hits
  // the ground.  In practice, it is hard to achieve both of
  // these perfectly, but if you can get close, your constant
  // will probably be good enough for balancing.
  int32_t diff = angleRate * ANGLE_RATE_RATIO - angle;
  if (diff > 0)
  {
    // On the top side, or pushed from the top side over to the
    // bottom.
    ledYellow(1);
    ledGreen(0);
  }
  else
  {
    // On the bottom side, or pushed from the bottom over to the
    // top.
    ledYellow(0);
    ledGreen(1);
  }
}
