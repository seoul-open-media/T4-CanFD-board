#include <Wire.h>
#include "RCB_Display.h"

void setup()
{ 
  Serial.begin(115200);
  initDisplay();
  displayControlBaordData();
  Serial.print("End of Setup");
}

void loop() {

}
