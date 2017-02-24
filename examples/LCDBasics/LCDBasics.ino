// This example demonstrates basic use of the Romi 32U4 LCD.  It
// prints the word "hi" on the first line of the LCD and prints
// the number 1234 on the second line.

#include <Romi32U4.h>

Romi32U4LCD lcd;

void setup()
{

}

void loop()
{
  // Clear the screen.
  lcd.clear();

  // Print a string.
  lcd.print("hi");

  // Go to the next line.
  lcd.gotoXY(0, 1);

  // Print a number.
  lcd.print(1234);

  delay(1000);
}

