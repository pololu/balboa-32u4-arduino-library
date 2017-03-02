#include <Wire.h>
#include "Balance.h"

int32_t gYZero;
int32_t angle; // millidegrees
int32_t angleRate; // degrees/s
int32_t distanceLeft;
int32_t speedLeft;
int32_t driveLeft;
int32_t distanceRight;
int32_t speedRight;
int32_t driveRight;
int16_t motorSpeed;
bool isBalancingStatus;
bool balanceUpdateDelayedStatus;

bool isBalancing()
{
  return isBalancingStatus;
}

bool balanceUpdateDelayed()
{
  return balanceUpdateDelayedStatus;
}

void balance()
{
  // drift toward angle=0 with timescale ~10s
  angle = angle * 999 / 1000;

  int32_t diff = angleRate * ANGLE_RATE_RATIO + angle;

  motorSpeed += (
    + ANGLE_RESPONSE * diff
    + DISTANCE_RESPONSE * (distanceLeft + distanceRight)
    + SPEED_RESPONSE * (speedLeft + speedRight)
    ) / 100 / GEAR_RATIO;

  if (motorSpeed > MOTOR_SPEED_LIMIT)
  {
    motorSpeed = MOTOR_SPEED_LIMIT;
  }
  if (motorSpeed < -MOTOR_SPEED_LIMIT)
  {
    motorSpeed = -MOTOR_SPEED_LIMIT;
  }

  int16_t distanceDiff = distanceLeft - distanceRight;

  motors.setSpeeds(motorSpeed - distanceDiff / 2,
    motorSpeed + distanceDiff / 2);
}

void lyingDown()
{
  // reset things so it doesn't go crazy
  motorSpeed = 0;
  distanceLeft = 0;
  distanceRight = 0;
  motors.setSpeeds(0, 0);

  if (angleRate > -2 && angleRate < 2)
  {
    // it's really calm, so we know the angles
    if(imu.a.z > 0)
    {
      angle = 110000;
    }
    else
    {
      angle = -110000;
    }
    distanceLeft = 0;
    distanceRight = 0;
  }
}

void integrateGyro()
{
  // convert from full-scale 1000 deg/s to deg/s
  angleRate = (imu.g.y - gYZero) / 29;

  angle += angleRate * UPDATE_TIME_MS;
}

void integrateEncoders()
{
  static int16_t lastCountsLeft;
  int16_t countsLeft = encoders.getCountsLeft();
  speedLeft = (countsLeft - lastCountsLeft);
  distanceLeft += countsLeft - lastCountsLeft;
  lastCountsLeft = countsLeft;

  static int16_t lastCountsRight;
  int16_t countsRight = encoders.getCountsRight();
  speedRight = (countsRight - lastCountsRight);
  distanceRight += countsRight - lastCountsRight;
  lastCountsRight = countsRight;
}

void balanceDrive(int16_t left, int16_t right)
{
  driveLeft = left;
  driveRight = right;
}

void balanceDoDriveTicks() {
  distanceLeft += driveLeft;
  distanceRight += driveRight;
  speedLeft += driveLeft;
  speedRight += driveRight;
}

void balanceSetup()
{
  // initialize IMU
  Wire.begin();
  if (!imu.init())
  {
    while(true)
    {
      Serial.println("Failed to detect and initialize IMU!");
      delay(200);
    }
  }
  imu.enableDefault();
  imu.writeReg(LSM6::CTRL2_G, 0b01011000); // 208 Hz, 1000 deg/s

  // wait for IMU readings to stabilize
  delay(1000);

  // calibrate the gyro
  int32_t total = 0;
  for (int i = 0; i < CALIBRATION_ITERATIONS; i++)
  {
    imu.read();
    total += imu.g.y;
    delay(1);
  }

  gYZero = total / CALIBRATION_ITERATIONS;
}

void balanceResetEncoders()
{
  distanceLeft = 0;
  distanceRight = 0;
}

void balanceUpdateSensors()
{
  imu.read();
  integrateGyro();
  integrateEncoders();
}

void balanceUpdate()
{
  static uint16_t lastMillis;
  uint16_t ms = millis();

  // lock our balancing updates to 100 Hz
  if(ms - lastMillis < UPDATE_TIME_MS) { return; }
  balanceUpdateDelayedStatus = ms - lastMillis > UPDATE_TIME_MS + 1;
  lastMillis = ms;

  balanceUpdateSensors();
  balanceDoDriveTicks();

  if(imu.a.x < 0)
  {
    lyingDown();
    isBalancingStatus = false;
  }
  else
  {
    balance();
    isBalancingStatus = true;
  }
}
