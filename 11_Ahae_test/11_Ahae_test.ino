#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_NeoKey_1x4.h>
#include <seesaw_neopixel.h>
#include <Metro.h>
#include <ACAN2517FD.h>
#include <Moteus.h>


#define IMU_SERIAL Serial2 // T4 pin 16, 17, micromod Serial2
#define LPS_SERIAL Serial6
#define SBUF_SIZE 64

#define PWM_Pin1 3
#define PWM_Pin2 4
#define PWM_Pin3 5
#define PWM_Pin4 6
#define FOOT_SW_L_PIN 23
#define FOOT_SW_R_PIN 29

static const byte MCP2517_SCK = 13 ; // SCK input of MCP2517
static const byte MCP2517_SDI =  11 ; // SDI input of MCP2517
static const byte MCP2517_SDO =  12 ; // SDO output of MCP2517

static const byte MCP2517_CS  = 10 ; // CS input of MCP2517
static const byte MCP2517_INT = 41 ; // INT output of MCP2517

static uint32_t gNextSendMillis = 0;

byte distance_data [3] = {0, 0, 0};
boolean keyPressed[4] = { false, false, false, false};
char sbuf[SBUF_SIZE];
signed int sbuf_cnt = 0;
float euler[3];
boolean fs_value[2] = {0, 0};



Metro neoKeyMetro = Metro (100);
Metro serialMetro = Metro (100);
Metro fsMetro = Metro (10);

Adafruit_NeoKey_1x4 neokey; // wire0

ACAN2517FD can (MCP2517_CS, SPI, MCP2517_INT) ;

Moteus moteus1(can, []() {
  Moteus::Options options;
  options.id = 1;
  return options;
}());
Moteus moteus2(can, []() {
  Moteus::Options options;
  options.id = 2;
  return options;
}());

Moteus::PositionMode::Command position_cmd;

void setup() {
  pinMode(PWM_Pin1, OUTPUT);
  pinMode(PWM_Pin2, OUTPUT);
  pinMode(PWM_Pin3, OUTPUT);
  pinMode(PWM_Pin4, OUTPUT);
  pinMode(FOOT_SW_L_PIN, INPUT);
  pinMode(FOOT_SW_R_PIN, INPUT);

  Serial.begin(115200);
  IMU_SERIAL.begin(57600);
  LPS_SERIAL.begin(9600);
  delay(1000);
  Wire.begin();
  Serial.println("NeoKey");

  if (neokey.begin(0x30)) {     // begin with I2C address, default is 0x30
    // enter configuration mode
    Serial.println("NeoKey started!");
  }
  Serial.println("NeoKey End");
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

  SPI.begin();

  // This operates the CAN-FD bus at 1Mbit for both the arbitration
  // and data rate.  Most arduino shields cannot operate at 5Mbps
  // correctly, so the moteus Arduino library permanently disables
  // BRS.
  ACAN2517FDSettings settings(
    ACAN2517FDSettings::OSC_40MHz, 1000ll * 1000ll, DataBitRateFactor::x1);

  // The atmega32u4 on the CANbed has only a tiny amount of memory.
  // The ACAN2517FD driver needs custom settings so as to not exhaust
  // all of SRAM just with its buffers.
  settings.mArbitrationSJW = 2;
  settings.mDriverTransmitFIFOSize = 1;
  settings.mDriverReceiveFIFOSize = 2;

  const uint32_t errorCode = can.begin(settings, [] { can.isr(); });

//  while (errorCode != 0) {
//    Serial.print(F("CAN error 0x"));
//    Serial.println(errorCode, HEX);
//    delay(1000);
//  }

  // To clear any faults the controllers may have, we start by sending
  // a stop command to each.
  moteus1.SetStop();
  moteus2.SetStop();
  Serial.println(F("all stopped"));

  Serial.println("End of Setup");
  while (!Serial) {}
  Serial.println(F("started"));
}
uint16_t gLoopCount = 0;
void loop() {
  //////////////////////////////////////////////////////// Neokey




  if (neoKeyMetro.check() == 1) {
    uint8_t buttons = neokey.read();

    if (buttons & (1 << 0)) {
      //  Serial.println("Button A");
      keyPressed[0] = true;
      neokey.pixels.setPixelColor(0, 0xFF0000); // red
      digitalWrite(PWM_Pin1, HIGH);
    } else {
      keyPressed[0] = false;
      neokey.pixels.setPixelColor(0, 0);
      digitalWrite(PWM_Pin1, LOW);
    }

    if (buttons & (1 << 1)) {
      //  Serial.println("Button B");
      keyPressed[1] = true;
      neokey.pixels.setPixelColor(1, 0xFFFF00); // yellow
      digitalWrite(PWM_Pin2, HIGH);
    } else {
      keyPressed[1] = false;
      neokey.pixels.setPixelColor(1, 0);
      digitalWrite(PWM_Pin2, LOW);
    }

    if (buttons & (1 << 2)) {
      // Serial.println("Button C");
      keyPressed[2] = true;
      neokey.pixels.setPixelColor(2, 0x00FF00); // green
      digitalWrite(PWM_Pin3, HIGH);
    } else {
      keyPressed[2] = false;
      neokey.pixels.setPixelColor(2, 0);
      digitalWrite(PWM_Pin3, LOW);
    }

    if (buttons & (1 << 3)) {
      // Serial.println("Button D");
      keyPressed[3] = true;
      neokey.pixels.setPixelColor(3, 0x00FFFF); // blue
      digitalWrite(PWM_Pin4, HIGH);
    } else {
      keyPressed[3] = false;
      neokey.pixels.setPixelColor(3, 0);
      digitalWrite(PWM_Pin4, LOW);
    }
    neokey.pixels.show();
  }

  //////////////////////////////////////////////////////// EBIMU
  if (EBimuAsciiParser(euler, 3))
  {
  }
  //////////////////////////////////////////////////////// LPS
  if (LPS_SERIAL.available() > 5) {
    if (LPS_SERIAL.read() == 255 && LPS_SERIAL.read() == 2) {
      for (int i = 0; i < 3; i++) {
        distance_data[i] = LPS_SERIAL.read();
      }
      byte elapsed_time = LPS_SERIAL.read();

    } else {
      while (LPS_SERIAL.available())LPS_SERIAL.read();

    }
  }
  //////////////////////////////////////////////////////// Foot Switch L, R
  if (fsMetro.check() == 1) {
    fs_value[0] = digitalRead(FOOT_SW_L_PIN);
    fs_value[1] = digitalRead(FOOT_SW_R_PIN);
  }

  //////////////////////////////////////////////////////// Moteus
  // We intend to send control frames every 20ms.
  const auto time = millis();
  if (gNextSendMillis >= time) {
    return;
  }

  gNextSendMillis += 20;
  gLoopCount++;

  Moteus::PositionMode::Command cmd;
  cmd.position = NaN;
  cmd.velocity = 0.2 * ::sin(time / 1000.0);

  moteus1.SetPosition(cmd);
  moteus2.SetBrake();

  if (gLoopCount % 5 != 0) {
    return;
  }
  // Only print our status every 5th cycle, so every 1s.

  Serial.print(F("time "));
  Serial.print(gNextSendMillis);

  auto print_moteus = [](const Moteus::Query::Result & query) {
    Serial.print(static_cast<int>(query.mode));
    Serial.print(F(" "));
    Serial.print(query.position);
    Serial.print(F("  velocity "));
    Serial.print(query.velocity);
  };

  print_moteus(moteus1.last_result().values);
  Serial.print(F(" / "));
  print_moteus(moteus2.last_result().values);
  Serial.println();

  ///////////////////////////////////////////////////////Serial.print
  if (serialMetro.check() == 1) {
    Serial.print("\n\r");
    Serial.print("NEOKEY ");
    Serial.print(keyPressed[0]);   Serial.print(" ");
    Serial.print(keyPressed[1]);   Serial.print(" ");
    Serial.print(keyPressed[2]);   Serial.print(" ");
    Serial.print(keyPressed[3]);   Serial.print(" ");

    Serial.print(" IMU ");
    Serial.print(euler[0]);   Serial.print(" ");
    Serial.print(euler[1]);   Serial.print(" ");
    Serial.print(euler[2]);   Serial.print(" ");

    Serial.print(" LPS ");
    Serial.print(distance_data[0]);   Serial.print(" ");
    Serial.print(distance_data[1]);   Serial.print(" ");
    Serial.print(distance_data[2]);   Serial.print(" ");

    Serial.print(" Foot Switch ");
    Serial.print(fs_value[0]);   Serial.print(" ");
    Serial.print(fs_value[1]);   Serial.print(" ");
  }

}
