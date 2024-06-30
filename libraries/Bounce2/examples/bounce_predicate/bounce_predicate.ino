
/*
  DESCRIPTION
  ====================
  Simple example of the Bounce library using the Adafruit NeoKey 1x4 that switches a LED when
  a the corresponding key state change (from HIGH to LOW) is triggered (for example when a button is pressed).

*/

#include <Bounce2.h>
#include "Adafruit_NeoKey_1x4.h"
#include "seesaw_neopixel.h"

Adafruit_NeoKey_1x4 neokey;

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
                                     PredicateDebouncer(&key_D_predicate, 5) };

// set a variable to store the led states
int led_states[4] = {LOW, LOW, LOW, LOW};

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Setup started");
  if (!neokey.begin(0x30)) {  // start matrix
    Serial.println("Could not start NeoKeys, check wiring?");
    while(1) {
      delay(10);
    }
  }

  Serial.println("NeoKey started!");

  // led setup
  for (uint16_t i = 0; i < 4; i++) {
    neokey.pixels.setPixelColor(i, 0x0000FF);
    neokey.pixels.show();
    delay(100);
    neokey.pixels.setPixelColor(i, 0x000000);
    neokey.pixels.show();
    delay(100);
  }
  Serial.println("Setup finished");
}

void loop() {
  // Update the Bounce instance (you must do this every loop)
  for (uint8_t i = 0; i < 4; i++) {
    debouncers[i].update();
  }
  for (uint8_t i = 0; i < 4; i++) {
    if (debouncers[i].rose()) {
      Serial.print(i);
      Serial.println(" was pressed");
      uint32_t pixel_colour = neokey.pixels.getPixelColor(i);
      neokey.pixels.setPixelColor(i, pixel_colour ^ 0x00FF0000); // write the new led state
      neokey.pixels.show();
    }
  }



}
