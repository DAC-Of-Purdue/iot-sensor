// Start the code with the include section.
// The include could be either
// public modules, for example, Arduino.h
// or your own modules if you have code in multiple files.
#include <Arduino.h>

// After the include section, then it is a define part.
// You can define constants here.
// The naming convention is all calptial letter.
#define CLUB_ROOM 'abe3078'

// Next section is global variable.
// Global variables are tricky to use.
// Be sure that you know what you are doing!
u_int8_t counter = 0;

// The setup function in one of two mandatory functions.
// This function will run when power on only once.
// As the name suggest, all setup code should be included in this fucntion.
void setup()
{
  Serial.begin(9600); // setup serial terminal.
  // The number is called "buadrate"
  // It indicates how many characters communication per second
}

// This is a loop function.
// Everything in this function will keep running forever.
void loop()
{
  counter++; // count how many time the loop executes
  Serial.print("Let's meet at "); // print message to the terminal
  Serial.println(CLUB_ROOM);
  delay(1000); // delay 1 second
}