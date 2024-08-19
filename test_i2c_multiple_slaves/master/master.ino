#include <Wire.h>

int DO_Blink = 13;
byte I2C_OnOff;
String readString;
String compareString;
bool toggle = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  for (int i = 0; i < 10; i++) {
    digitalWrite(DO_Blink, HIGH);
    delay(100);
    digitalWrite(DO_Blink, LOW);
    delay(200);
  }

  compareString = String("1");
}

void loop()
{
  while (Serial.available()) {
    delay(2);
    char c = Serial.read();
    readString += c;
  }

  if (readString.length() <= 0) return;

  if (readString.equals(compareString)) {
    I2C_OnOff = 1;
  } else {
    I2C_OnOff = 0;
  }
  
  Serial.print(F("You entered: "));
  Serial.println(readString);
  Serial.print(F("I2C_OnOff = "));
  Serial.println(I2C_OnOff);

  readString = "";

  toggle = !toggle;
  Wire.beginTransmission(1);
  Wire.write(toggle ? 1 : 0);
  Wire.endTransmission();
}
