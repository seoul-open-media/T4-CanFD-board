// Get the wireless data from master Xbee and print/display
// Xbees configured to Mesh and baudrate is 115200
// Xbee config files are,
// https://drive.google.com/drive/folders/1PO4C6Iw2v6WuFPc-lhFs-_ZEKmQdtRPI?usp=drive_link

#include <Wire.h>
#include "RCB_Display.h"

#define XBEESERIAL Serial4 // T4 pin 7, 8, micromod Serial4


void setup() {
  Serial.begin(115200);
  XBEESERIAL.begin(115200);
  initDisplay();
  displayControlBaordData();

  clearDisplay();
  display.println("Data from Xbee");
  display.display();
  Serial.println("End of Setup");
  XBEESERIAL.write(255);
  XBEESERIAL.write(255);
  XBEESERIAL.write(255);
  XBEESERIAL.write(255);
  XBEESERIAL.write(1);
  unsigned long lastTimeSent;
  //if (millis() > lastTimeSent + 1000) {

  // XBEESERIAL.write(1); XBEESERIAL.write(2); XBEESERIAL.write(3);
  /*   for (int i = 0; i < 10; i++) {
       XBEESERIAL.write(i);
     }
     lastTimeSent = millis();
  */
  // }

}

void loop() {
  if (XBEESERIAL.available()) {
    char data = XBEESERIAL.read();
    Serial.println(data);
    display.print(data);
    display.display();
  }

    unsigned long lastTimeSent;
    if (millis() > lastTimeSent + 1000) {
      XBEESERIAL.write(255); XBEESERIAL.write(255); XBEESERIAL.write(255); XBEESERIAL.write(255);
      for (int i = 0; i < 10; i++) {
      //  XBEESERIAL.write(i);
      }
      lastTimeSent = millis();
    }

}
