#include <Wire.h>

int DO_Blink = 13;
byte I2C_OnOff;

void setup() {
  Wire.begin(1);
  Wire.onReceive(BlinkLED);

  pinMode(DO_Blink, OUTPUT);

  for (int i = 0; i < 10; i++) {
    digitalWrite(DO_Blink, HIGH);
    delay(100);
    digitalWrite(DO_Blink, LOW);
    delay(200);
  }
}

void loop() {
  delay(100);
}

void BlinkLED(int Press) {
  Serial.println(F("onReceive"));

  I2C_OnOff = Wire.read();

  if (I2C_OnOff == 1) {
    digitalWrite(DO_Blink, HIGH);
  } else if (I2C_OnOff == 0) {
    digitalWrite(DO_Blink, LOW);
  }
}
