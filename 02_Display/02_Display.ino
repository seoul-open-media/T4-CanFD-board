#include <Wire.h>
#include "RCB_Display.h"

void setup()
{ 
  Serial.begin(115200);
  initDisplay();
  displayControlBaordData();
  Serial.println("End of Setup");
  clearDisplay();
  display.println("Hi");
  display.display();
}

void loop() {
}
