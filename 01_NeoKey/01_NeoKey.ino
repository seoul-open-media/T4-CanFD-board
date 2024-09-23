#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_NeoKey_1x4.h>
#include <seesaw_neopixel.h>
#include <Bounce2.h>
#include <Metro.h>

Adafruit_NeoKey_1x4 neokey; // changed library to wire1
Metro neoKeyMetro = Metro(100);

//-----------neoKey-----------------------------
boolean keyPressed[4] = {false, false, false, false};
boolean lastKeyState[4] = {false, false, false, false};
uint8_t neoKeyNum = 0;
bool key_A_predicate()
{
  return neokey.read() & 0x01;
}

bool key_B_predicate()
{
  return neokey.read() & 0x02;
}

bool key_C_predicate()
{
  return neokey.read() & 0x04;
}

bool key_D_predicate()
{
  return neokey.read() & 0x08;
}

// instantiate the PrediateDebouncer objects
PredicateDebouncer debouncers[4] = { PredicateDebouncer(&key_A_predicate, 5),
                                     PredicateDebouncer(&key_B_predicate, 5),
                                     PredicateDebouncer(&key_C_predicate, 5),
                                     PredicateDebouncer(&key_D_predicate, 5)
                                   };

void neoKey() {
  //Serial.println("neoKey");
  for (uint8_t i = 0; i < 4; i++) {
    debouncers[i].update();
  }

  if (debouncers[0].rose()) {
    Serial.println("1 preseed.");
  }
  if (debouncers[1].rose()) {

    Serial.println("2 preseed.");
  }

  if (debouncers[2].rose()) {
    Serial.println("3 preseed.");
  }
  if (debouncers[3].rose()) {
    Serial.println("4 preseed.");
  }

}
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Wire.begin();
  //  Wire.setClock(400000);
 // Wire1.begin();
  delay(1000);

  if (neokey.begin(0x30)) {     // begin with I2C address, default is 0x30
    // enter configuration mode
    Serial.println("NeoKey started!");
  }

  Serial.println("end of setup");


}

void loop() {
  // put your main code here, to run repeatedly:

  if (neoKeyMetro.check() == 1) {
    neoKey();
  }

}
