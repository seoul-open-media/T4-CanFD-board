#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_NeoKey_1x4.h>
#include <seesaw_neopixel.h>
#include <Bounce2.h>
#include <Metro.h>

Adafruit_NeoKey_1x4 neokey; // changed library to wire1
Metro neoKeyMetro = Metro(100);

const int pwmPin1 = 22;
const int a1Pin1 = 21;
const int b1Pin1 = 20;
const int pwmPin2 = 23;
const int a1Pin2 = 29;
const int b1Pin2 = 28;

int left_leg = 0;
int right_leg = 0;

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
    if (left_leg == 0) {
      digitalWrite(a1Pin1, 1);
      digitalWrite(pwmPin1, 1);
      left_leg = 1;
    } else {

      digitalWrite(a1Pin1, 0);
      digitalWrite(pwmPin1, 0);
      left_leg = 0;
    }
  }
  if (debouncers[1].rose()) {
    if (right_leg == 0) {
      digitalWrite(a1Pin2, 1);
      digitalWrite(pwmPin2, 1);
      right_leg = 1;
    } else {
      digitalWrite(a1Pin2, 0);
      digitalWrite(pwmPin2, 0);

      right_leg = 0;
    }
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

  pinMode(pwmPin1, OUTPUT);
  pinMode(a1Pin1, OUTPUT);
  pinMode(b1Pin1, OUTPUT);
  pinMode(pwmPin2, OUTPUT);
  pinMode(a1Pin2, OUTPUT);
  pinMode(b1Pin2, OUTPUT);
  digitalWrite(pwmPin1, 0);
  digitalWrite(a1Pin1, 0);
  digitalWrite(b1Pin1, 0);
  digitalWrite(pwmPin2, 0);
  digitalWrite(a1Pin2, 0);
  digitalWrite(b1Pin2, 0);

  Serial.begin(115200);
  Wire.begin();
  //  Wire.setClock(400000);
  Wire1.begin();
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
