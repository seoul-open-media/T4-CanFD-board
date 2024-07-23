#include <Wire.h>
#include "RCB_Display.h"

void setup()
{ 
  Serial.begin(115200);
  initDisplay();
  displayControlBaordData();
  Serial.println("End of Setup");
  clearDisplay();
  display.println("T4 CanFD v.1.5");
  display.display();
}

void loop() {
}
