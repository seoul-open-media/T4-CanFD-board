#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_NeoKey_1x4.h>
#include <seesaw_neopixel.h>
#include <Bounce2.h>
#include <Metro.h>
#include "RCB_Display.h"

#define IMU_SERIAL Serial2 // T4 pin 16, 17, micromod Serial2
#define SBUF_SIZE 64

Adafruit_NeoKey_1x4 neokey; // wire0

Metro displayMetro = Metro(100);
Metro neoKeyMetro = Metro (100);

boolean keyPressed[4] = { false, false, false, false};
char sbuf[SBUF_SIZE];
signed int sbuf_cnt = 0;
float euler[3];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
 
  IMU_SERIAL.begin(57600); 
  Wire.begin();
  //  Wire.setClock(400000);
  Wire1.begin();
  delay(1000);

  if (neokey.begin(0x30)) {     // begin with I2C address, default is 0x30
    // enter configuration mode
    Serial.println("NeoKey started!");
  }

  initDisplay();
  displayControlBaordData();
  Serial.println("End of Setup");
  clearDisplay();
  display.println("T4 CanFD v.1.5");
  display.display();
  delay(2000);
  Serial.println("end of setup");


  // Pulse all the LEDs on to show we're working
  for (uint16_t i = 0; i < neokey.pixels.numPixels(); i++) {
    neokey.pixels.setPixelColor(i, 0x808080); // make each LED white
    neokey.pixels.show();
    delay(50);
  }
  for (uint16_t i = 0; i < neokey.pixels.numPixels(); i++) {
    neokey.pixels.setPixelColor(i, 0x000000);
    neokey.pixels.show();
    delay(50);
  }

}

void loop() {
  //////////////////////////////////////////////////////// Neokey
  uint8_t buttons = neokey.read();

  if (buttons & (1 << 0)) {
    //  Serial.println("Button A");
    keyPressed[0] = true;
    neokey.pixels.setPixelColor(0, 0xFF0000); // red
  } else {
    keyPressed[0] = false;
    neokey.pixels.setPixelColor(0, 0);
  }

  if (buttons & (1 << 1)) {
    //  Serial.println("Button B");
    keyPressed[1] = true;
    neokey.pixels.setPixelColor(1, 0xFFFF00); // yellow
  } else {
    keyPressed[1] = false;
    neokey.pixels.setPixelColor(1, 0);
  }

  if (buttons & (1 << 2)) {
    // Serial.println("Button C");
    keyPressed[2] = true;
    neokey.pixels.setPixelColor(2, 0x00FF00); // green
  } else {
    keyPressed[2] = false;
    neokey.pixels.setPixelColor(2, 0);
  }

  if (buttons & (1 << 3)) {
    // Serial.println("Button D");
    keyPressed[3] = true;
    neokey.pixels.setPixelColor(3, 0x00FFFF); // blue
  } else {
    keyPressed[3] = false;
    neokey.pixels.setPixelColor(3, 0);
  }



  if (neoKeyMetro.check() == 1) {
    neokey.pixels.show();
  }
  //////////////////////////////////////////////////////// EBIMU
  if (EBimuAsciiParser(euler, 3))
  {
    Serial.print("\n\r");
    Serial.print(euler[0]);   Serial.print(" ");
    Serial.print(euler[1]);   Serial.print(" ");
    Serial.print(euler[2]);   Serial.print(" ");

    //     clearDisplay();
    //     display.print(euler[0]);   display.print(" ");
    //     display.print(euler[1]);   display.print(" ");
    //     display.print(euler[2]);   display.print(" ");
    //     display.display();
  }



  //////////////////////////////////////////////////////// Display
  if (displayMetro.check() == 1) {


    clearDisplay();
    display.print("1:"); display.print(keyPressed[0]);
    display.print(", 2:"); display.print(keyPressed[1]);
    display.print(", 3:"); display.print(keyPressed[2]);
    display.print(", 4:"); display.print(keyPressed[3]);
    display.println("EBIMU");
    display.print(euler[0]);   display.print(" ");
    display.print(euler[1]);   display.print(" ");
    display.print(euler[2]);   display.print(" ");
    display.println();
    display.display();


  }
}
