#include <ACAN2517FD.h>
#include <Adafruit_NeoKey_1x4.h>
#include <Arduino.h>
#include <Bounce2.h>
#include <Metro.h>
#include <Moteus.h>
#include <Wire.h>
#include <seesaw_neopixel.h>

/// Setup SPI connection between Teensy and MCP2518
static const byte MCP2517_SCK = 13;  // SCK input of MCP2517
static const byte MCP2517_SDI = 11;  // SDI input of MCP2517
static const byte MCP2517_SDO = 12;  // SDO output of MCP2517
static const byte MCP2517_CS = 10;   // CS input of MCP2517
static const byte MCP2517_INT = 41;  // INT output of MCP2517
ACAN2517FD can(MCP2517_CS, SPI, MCP2517_INT);

/// Initialize Moteus object for ID 1
Moteus moteus1(can, []() {
  Moteus::Options options;
  options.id = 1;
  return options;
}());

/// Utility printer for Moteus Reply
void print_moteus(const Moteus::Query::Result& reply) {
  Serial.print(static_cast<int>(reply.mode));
  Serial.print(F(" "));
  Serial.print(reply.position);
  Serial.print(F("  velocity "));
  Serial.print(reply.velocity);
  Serial.print(F("  fault "));
  Serial.print(reply.fault);
  Serial.println();
};

/// Setup NeoKey1x4 and its metronome
Adafruit_NeoKey_1x4 neokey;  // changed library to wire1
Metro neoKeyMetro = Metro(100);
PredicateDebouncer debouncers[4] = {
    {[] -> bool { return neokey.read() & 0x01; }, 5},
    {[] -> bool { return neokey.read() & 0x02; }, 5},
    {[] -> bool { return neokey.read() & 0x04; }, 5},
    {[] -> bool { return neokey.read() & 0x08; }, 5}};

/// NeoKey1x4 input update and handler function
void neoKey() {
  for (uint8_t i = 0; i < 4; i++) {
    debouncers[i].update();
    if (debouncers[i].rose()) {
      Serial.print(i);
      Serial.println(" pressed");
      moteus1.SetPosition(Moteus::PositionMode::Command{.position = 0.25 * i});
    }
  }
}

/// Timer for Reply printing
static uint32_t gNextPrintMillis = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    Serial.println(F("Serial problem"));
    delay(1000);
  }
  Serial.println(F("started"));

  Wire.begin();
  Wire1.begin();
  delay(1000);
  if (neokey.begin(0x30)) {  // begin with I2C address, default is 0x30
    Serial.println("NeoKey started!");
  }

  SPI.begin();

  // This operates the CAN-FD bus at 1Mbit for both the arbitration
  // and data rate.  Most arduino shields cannot operate at 5Mbps
  // correctly, so the moteus Arduino library permanently disables
  // BRS.
  ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_40MHz, 1000ll * 1000ll,
                              DataBitRateFactor::x1);
  // The atmega32u4 on the CANbed has only a tiny amount of memory.
  // The ACAN2517FD driver needs custom settings so as to not exhaust
  // all of SRAM just with its buffers.
  settings.mArbitrationSJW = 2;
  settings.mDriverTransmitFIFOSize = 1;
  settings.mDriverReceiveFIFOSize = 2;
  const uint32_t errorCode = can.begin(settings, [] { can.isr(); });
  while (errorCode != 0) {
    Serial.print(F("CAN error 0x"));
    Serial.println(errorCode, HEX);
    delay(1000);
  }

  moteus1.SetStop();
  Serial.println(F("all stopped"));
}

void loop() {
  if (neoKeyMetro.check() == 1) {
    neoKey();
  }

  if (millis() > gNextPrintMillis) {
    gNextPrintMillis += 1000;
    Serial.print(F("time "));
    Serial.print(gNextPrintMillis);
    print_moteus([&] {
      moteus1.SetQuery();
      return moteus1.last_result().values;
    }());
  }
}
