// This demo program shows many features of the Romi 32U4.
//
// It uses the buttons, LCD, and buzzer to provide a user
// interface.  It presents a menu to the user that lets the user
// select from several different demos.
//
// To use this demo program, you will need to have the LCD
// connected to the Romi 32U4.  If you cannot see any text on the
// LCD, try rotating the contrast potentiometer.
//
// To run this sketch, you will need to install the LSM6 library:
//
// https://github.com/pololu/lsm6-arduino

#include <Wire.h>
#include <Romi32U4.h>
#include <LSM6.h>

Romi32U4LCD lcd;
Romi32U4Buzzer buzzer;
Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;
LSM6 imu;
Romi32U4Motors motors;
Romi32U4Encoders encoders;

char buttonMonitor();

// A couple of simple tunes, stored in program space.
const char beepBrownout[] PROGMEM = "<c8";
const char beepWelcome[] PROGMEM = ">g32>>c32";
const char beepThankYou[] PROGMEM = ">>c32>g32";
const char beepButtonA[] PROGMEM = "!c32";
const char beepButtonB[] PROGMEM = "!e32";
const char beepButtonC[] PROGMEM = "!g32";

// A longer song and its title.
const char fugue[] PROGMEM =
  "! T120O5L16agafaea dac+adaea fa<aa<bac#a dac#adaea f"
  "O6dcd<b-d<ad<g d<f+d<gd<ad<b- d<dd<ed<f+d<g d<f+d<gd<ad"
  "L8MS<b-d<b-d MLe-<ge-<g MSc<ac<a MLd<fd<f O5MSb-gb-g"
  "ML>c#e>c#e MS afaf ML gc#gc# MS fdfd ML e<b-e<b-"
  "O6L16ragafaea dac#adaea fa<aa<bac#a dac#adaea faeadaca"
  "<b-acadg<b-g egdgcg<b-g <ag<b-gcf<af dfcf<b-f<af"
  "<gf<af<b-e<ge c#e<b-e<ae<ge <fe<ge<ad<fd"
  "O5e>ee>ef>df>d b->c#b->c#a>df>d e>ee>ef>df>d"
  "e>d>c#>db>d>c#b >c#agaegfe fO6dc#dfdc#<b c#4";

const char fugueTitle[] PROGMEM =
  "       Fugue in D Minor - by J.S. Bach       ";

// This LCD custom character is a back arrow.
const char backArrow[] PROGMEM = {
  0b00000,
  0b00010,
  0b00001,
  0b00101,
  0b01001,
  0b11110,
  0b01000,
  0b00100,
};

// This LCD custom character is two chevrons pointing up.
const char forwardArrows[] PROGMEM = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
};

// This LCD custom character is two chevrons pointing down.
const char reverseArrows[] PROGMEM = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b10001,
  0b01010,
  0b00100,
  0b00000,
};

// This LCD custom character is two solid arrows pointing up.
const char forwardArrowsSolid[] PROGMEM = {
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
};

// This LCD custom character is two solid arrows pointing down.
const char reverseArrowsSolid[] PROGMEM = {
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
};

void loadCustomCharacters()
{
  // The LCD supports up to 8 custom characters.  Each character
  // has a number between 0 and 7.  We assign #7 to be the back
  // arrow; other characters are loaded by individual demos as
  // needed.

  lcd.loadCustomCharacter(backArrow, 7);
}

// Assigns #0-6 to be bar graph characters.
void loadCustomCharactersBarGraph()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  lcd.loadCustomCharacter(levels + 0, 0);  // 1 bar
  lcd.loadCustomCharacter(levels + 1, 1);  // 2 bars
  lcd.loadCustomCharacter(levels + 2, 2);  // 3 bars
  lcd.loadCustomCharacter(levels + 3, 3);  // 4 bars
  lcd.loadCustomCharacter(levels + 4, 4);  // 5 bars
  lcd.loadCustomCharacter(levels + 5, 5);  // 6 bars
  lcd.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

// Assigns #0-4 to be arrow symbols.
void loadCustomCharactersMotorDirs()
{
  lcd.loadCustomCharacter(forwardArrows, 0);
  lcd.loadCustomCharacter(reverseArrows, 1);
  lcd.loadCustomCharacter(forwardArrowsSolid, 2);
  lcd.loadCustomCharacter(reverseArrowsSolid, 3);
}

// Clears the LCD and puts [back_arrow]B on the second line
// to indicate to the user that the B button goes back.
void displayBackArrow()
{
  lcd.clear();
  lcd.gotoXY(0,1);
  lcd.print(F("\7B"));
  lcd.gotoXY(0,0);
}

// Prints a bar graph character on the LCD using the custom
// characters provided by loadCustomCharacterBarGraph and some
// special characters supported by the HD44780.
void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  static const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, 255};
  lcd.print(barChars[height]);
}

// The Menu class shows an interactive menu on the screen that
// lets a user select an action and keeps track of the menu's
// state.
class Menu
{
public:
  struct Item
  {
    const char * name;
    void (* action)();
  };

  Menu(Item * items, uint8_t itemCount)
  {
    this->items = items;
    this->itemCount = itemCount;
    lcdItemIndex = 0;
  }

  void lcdUpdate(uint8_t index)
  {
    lcd.clear();
    lcd.print(items[index].name);
    lcd.gotoXY(0, 1);

    // The string below uses special characters from the HD44780
    // interface datasheet.  "\x7f" is a left arrow.
    // "\xa5" is a dot character.  "\x7e" is a right arrow.
    lcd.print(F("\x7f" "A \xa5" "B C\x7e"));
  }

  void action(uint8_t index)
  {
    items[index].action();
  }

  // Prompts the user to choose one of the menu items,
  // then runs it, then returns.
  void select()
  {
    lcdUpdate(lcdItemIndex);

    while (1)
    {
      switch (buttonMonitor())
      {
      case 'A':
        // The A button was pressed so decrement the index.
        if (lcdItemIndex == 0)
        {
          lcdItemIndex = itemCount - 1;
        }
        else
        {
          lcdItemIndex--;
        }
        lcdUpdate(lcdItemIndex);
        break;

      case 'C':
        // The C button was pressed so increase the index.
        if (lcdItemIndex >= itemCount - 1)
        {
          lcdItemIndex = 0;
        }
        else
        {
          lcdItemIndex++;
        }
        lcdUpdate(lcdItemIndex);
        break;

      case 'B':
        // The B button was pressed so run the item and return.
        action(lcdItemIndex);
        return;
      }
    }
  }

private:
  Item * items;
  uint8_t itemCount;
  uint8_t lcdItemIndex;
};


// Blinks all three LEDs in sequence.
void ledDemo()
{
  displayBackArrow();

  uint8_t state = 3;
  static uint16_t lastUpdateTime = millis() - 2000;
  while (buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastUpdateTime) >= 500)
    {
      lastUpdateTime = millis();
      state = state + 1;
      if (state >= 4) { state = 0; }

      switch (state)
      {
      case 0:
        buzzer.play("c32");
        lcd.gotoXY(0, 0);
        lcd.print(F("Red   "));
        ledRed(1);
        ledGreen(0);
        ledYellow(0);
        break;

      case 1:
        buzzer.play("e32");
        lcd.gotoXY(0, 0);
        lcd.print(F("Green"));
        ledRed(0);
        ledGreen(1);
        ledYellow(0);
        break;

      case 2:
        buzzer.play("g32");
        lcd.gotoXY(0, 0);
        lcd.print(F("Yellow"));
        ledRed(0);
        ledGreen(0);
        ledYellow(1);
        break;
      }
    }
  }

  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}

// Starts I2C and initializes the inertial sensors.
void initInertialSensors()
{
  Wire.begin();
  imu.init();
  imu.enableDefault();

  // Set the gyro full scale to 1000 dps because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL2_G, 0b10001000);

  // Set the accelerometer full scale to 16 g because the default
  // value is too low, and leave the other settings the same.
  imu.writeReg(LSM6::CTRL1_XL, 0b10000100);
}

// Given 3 readings for axes x, y, and z, prints the sign
// and axis of the largest reading unless it is below the
// given threshold.
void printLargestAxis(int16_t x, int16_t y, int16_t z, uint16_t threshold)
{
  int16_t largest = x;
  char axis = 'X';

  if (abs(y) > abs(largest))
  {
    largest = y;
    axis = 'Y';
  }
  if (abs(z) > abs(largest))
  {
    largest = z;
    axis = 'Z';
  }

  if (abs(largest) < threshold)
  {
    lcd.print("  ");
  }
  else
  {
    bool positive = (largest > 0);
    lcd.print(positive ? '+' : '-');
    lcd.print(axis);
  }
}

// Print the direction of the largest rotation rate measured
// by the gyro and the up direction based on the
// accelerometer's measurement of gravitational acceleration
// (assuming gravity is the dominant force acting on the
// Romi).
void inertialDemo()
{
  displayBackArrow();

  lcd.gotoXY(3, 0);
  lcd.print(F("Rot"));
  lcd.gotoXY(4, 1);
  lcd.print(F("Up"));

  while (buttonMonitor() != 'B')
  {
    imu.read();

    lcd.gotoXY(6, 0);
    printLargestAxis(imu.g.x, imu.g.y, imu.g.z, 2000);
    lcd.gotoXY(6, 1);
    printLargestAxis(imu.a.x, imu.a.y, imu.a.z, 200);
  }
}

// Provides an interface to test the motors. Holding button A or C
// causes the left or right motor to accelerate; releasing the
// button causes the motor to decelerate. Tapping the button while
// the motor is not running reverses the direction it runs.
//
// If the showEncoders argument is true, encoder counts are
// displayed on the first line of the LCD; otherwise, an
// instructional message is shown.
void motorDemoHelper(bool showEncoders)
{
  loadCustomCharactersMotorDirs();
  lcd.clear();
  lcd.gotoXY(1, 1);
  lcd.print(F("A \7B C"));

  const uint16_t maxSpeed = 300;
  const uint8_t acceleration = 15;
  const uint8_t deceleration = 30;

  // Update the LCD and motors every 50 ms.
  const uint8_t updatePeriod = 50;

  int16_t leftSpeed = 0, rightSpeed = 0;
  int8_t leftDir = 1, rightDir = 1;
  uint16_t lastUpdateTime = millis() - 100;
  uint8_t btnCountA = 0, btnCountC = 0, instructCount = 0;

  int16_t encCountsLeft = 0, encCountsRight = 0;
  char buf[4];

  while (buttonMonitor() != 'B')
  {
    encCountsLeft += encoders.getCountsAndResetLeft();
    if (encCountsLeft < 0) { encCountsLeft += 1000; }
    if (encCountsLeft > 999) { encCountsLeft -= 1000; }

    encCountsRight += encoders.getCountsAndResetRight();
    if (encCountsRight < 0) { encCountsRight += 1000; }
    if (encCountsRight > 999) { encCountsRight -= 1000; }

    if ((uint16_t)(millis() - lastUpdateTime) > updatePeriod)
    {
      lastUpdateTime = millis();

      lcd.gotoXY(0, 0);
      if (showEncoders)
      {
        sprintf(buf, "%03d", encCountsLeft);
        lcd.print(buf);
        lcd.gotoXY(5, 0);
        sprintf(buf, "%03d", encCountsRight);
        lcd.print(buf);
      }
      else
      {
        // Cycle the instructions every 2 seconds.
        if (instructCount == 0)
        {
          lcd.print(F("Hold=run"));
        }
        else if (instructCount == 40)
        {
          lcd.print(F("Tap=flip"));
        }
        if (++instructCount == 80) { instructCount = 0; }
      }

      if (buttonA.isPressed())
      {
        if (btnCountA < 4)
        {
          btnCountA++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          leftSpeed += acceleration;
        }
      }
      else
      {
        if (leftSpeed == 0 && btnCountA > 0 && btnCountA < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          leftDir = -leftDir;
        }
        btnCountA = 0;
        leftSpeed -= deceleration;
      }

      if (buttonC.isPressed())
      {
        if (btnCountC < 4)
        {
          btnCountC++;
        }
        else
        {
          // Button has been held for more than 200 ms, so
          // start running the motor.
          rightSpeed += acceleration;
        }
      }
      else
      {
        if (rightSpeed == 0 && btnCountC > 0 && btnCountC < 4)
        {
          // Motor isn't running and button was pressed for
          // 200 ms or less, so flip the motor direction.
          rightDir = -rightDir;
        }
        btnCountC = 0;
        rightSpeed -= deceleration;
      }

      leftSpeed = constrain(leftSpeed, 0, maxSpeed);
      rightSpeed = constrain(rightSpeed, 0, maxSpeed);

      motors.setSpeeds(leftSpeed * leftDir, rightSpeed * rightDir);

      // Display arrows pointing the appropriate direction
      // (solid if the motor is running, chevrons if not).
      lcd.gotoXY(0, 1);
      if (leftSpeed == 0)
      {
        lcd.print((leftDir > 0) ? '\0' : '\1');
      }
      else
      {
        lcd.print((leftDir > 0) ? '\2' : '\3');
      }
      lcd.gotoXY(7, 1);
      if (rightSpeed == 0)
      {
        lcd.print((rightDir > 0) ? '\0' : '\1');
      }
      else
      {
        lcd.print((rightDir > 0) ? '\2' : '\3');
      }
    }
  }
  motors.setSpeeds(0, 0);
}


// Motor demo with instructions.
void motorDemo()
{
  motorDemoHelper(false);
}

// Motor demo with encoder counts.
void encoderDemo()
{
  motorDemoHelper(true);
}

// Play a song on the buzzer and display its title.
void musicDemo()
{
  displayBackArrow();

  uint8_t fugueTitlePos = 0;
  uint16_t lastShiftTime = millis() - 2000;

  while (buttonMonitor() != 'B')
  {
    // Shift the song title to the left every 250 ms.
    if ((uint16_t)(millis() - lastShiftTime) > 250)
    {
      lastShiftTime = millis();

      lcd.gotoXY(0, 0);
      for (uint8_t i = 0; i < 8; i++)
      {
        char c = pgm_read_byte(fugueTitle + fugueTitlePos + i);
        lcd.print(c);
      }
      fugueTitlePos++;

      if (fugueTitlePos + 8 >= strlen(fugueTitle))
      {
        fugueTitlePos = 0;
      }
    }

    if (!buzzer.isPlaying())
    {
      buzzer.playFromProgramSpace(fugue);
    }
  }
}

// Display the the battery (VIN) voltage and indicate whether USB
// power is detected.
void powerDemo()
{
  displayBackArrow();

  uint16_t lastDisplayTime = millis() - 2000;
  char buf[6];

  while (buttonMonitor() != 'B')
  {
    if ((uint16_t)(millis() - lastDisplayTime) > 250)
    {
      bool usbPower = usbPowerPresent();

      uint16_t batteryLevel = readBatteryMillivolts();

      lastDisplayTime = millis();
      lcd.gotoXY(0, 0);
      sprintf(buf, "%5d", batteryLevel);
      lcd.print(buf);
      lcd.print(F(" mV"));
      lcd.gotoXY(3, 1);
      lcd.print(F("USB="));
      lcd.print(usbPower ? 'Y' : 'N');
    }
  }
}

Menu::Item mainMenuItems[] = {
  { "LEDs", ledDemo },
  { "Inertial", inertialDemo },
  { "Motors", motorDemo },
  { "Encoders", encoderDemo },
  { "Music", musicDemo },
  { "Power", powerDemo },
};
Menu mainMenu(mainMenuItems, 6);

// This function watches for button presses.  If a button is
// pressed, it beeps a corresponding beep and it returns 'A',
// 'B', or 'C' depending on what button was pressed.  If no
// button was pressed, it returns 0.  This function is meant to
// be called repeatedly in a loop.
char buttonMonitor()
{
  if (buttonA.getSingleDebouncedPress())
  {
    buzzer.playFromProgramSpace(beepButtonA);
    return 'A';
  }

  if (buttonB.getSingleDebouncedPress())
  {
    buzzer.playFromProgramSpace(beepButtonB);
    return 'B';
  }

  if (buttonC.getSingleDebouncedPress())
  {
    buzzer.playFromProgramSpace(beepButtonC);
    return 'C';
  }

  return 0;
}

void setup()
{
  initInertialSensors();

  loadCustomCharacters();

  bool brownout = MCUSR >> BORF & 1;
  MCUSR = 0;
  if (brownout)
  {
    // The AVR was reset by a brownout reset
    // (VCC dropped below 4.3 V).
    // Play a special sound and display a note to the user.

    buzzer.playFromProgramSpace(beepBrownout);
    lcd.clear();
    lcd.print(F("Brownout"));
    lcd.gotoXY(0, 1);
    lcd.print(F(" reset! "));
    delay(1000);
  }
  else
  {
    buzzer.playFromProgramSpace(beepWelcome);
  }

  ledYellow(1);

  lcd.clear();
  lcd.print(F("  Romi"));
  lcd.gotoXY(2, 1);
  lcd.print(F("32U4"));

  delay(1000);

  lcd.clear();
  lcd.print(F("Demo"));
  lcd.gotoXY(0, 1);
  lcd.print(F("Program"));
  delay(1000);

  lcd.clear();
  lcd.print(F("Use B to"));
  lcd.gotoXY(0, 1);
  lcd.print(F("select."));
  delay(1000);

  lcd.clear();
  lcd.print(F("Press B"));
  lcd.gotoXY(0, 1);
  lcd.print(F("-try it!"));

  while (buttonMonitor() != 'B'){}

  ledYellow(0);

  buzzer.playFromProgramSpace(beepThankYou);
  lcd.clear();
  lcd.print(F(" Thank"));
  lcd.gotoXY(0, 1);
  lcd.print(F("  you!"));
  delay(1000);
}

// This function prompts the user to choose something from the
// main menu, runs their selection, and then returns.
void mainMenuSelect()
{
  lcd.clear();
  lcd.print(F("  Main"));
  lcd.gotoXY(0, 1);
  lcd.print(F("  Menu"));
  delay(1000);
  mainMenu.select();
}

void loop()
{
  mainMenuSelect();
}
