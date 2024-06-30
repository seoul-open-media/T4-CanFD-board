#include <SPI.h>
#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgTime.hpp>
#include <DW1000NgConstants.hpp>

#define master_address 255
#define LEN_DATA 20

byte data[LEN_DATA];

// connection pins
const uint8_t PIN_RST = 3; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = 10; // spi select pin

volatile boolean sentAck = false;
volatile boolean receivedAck = false;

device_configuration_t DEFAULT_CONFIG = {
  false,
  true,
  true,
  true,
  false,
  SFDMode::STANDARD_SFD,
  Channel::CHANNEL_5,
  DataRate::RATE_850KBPS,
  PulseFrequency::FREQ_16MHZ,
  PreambleLength::LEN_256,
  PreambleCode::CODE_3
};

interrupt_configuration_t DEFAULT_INTERRUPT_CONFIG = {
  true,
  true,
  true,
  false,
  true
};
void setup() {
  // pinMode(ledPin, OUTPUT);
  // DEBUG monitoring
  Serial.begin(9600);
  //delay(2000);
  //while (!Serial);

  Serial.println(F("### DW1000Ng-arduino-ranging-Initiator ###"));
  // initialize the driver
  DW1000Ng::initialize(PIN_SS, PIN_IRQ, PIN_RST);
  Serial.println("DW1000Ng initialized ...");
  // general configuration
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

  DW1000Ng::setDeviceAddress(master_address);
  DW1000Ng::setNetworkId(10);
  DW1000Ng::setAntennaDelay(16436);
  //delay(1000);
  DW1000Ng::setTXPower(522133279);  // 0x1F1F1F1F
  //DW1000Ng::setTXPower(2358021260);  // 0x8c8c8c8c

  Serial.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msg[128];
  DW1000Ng::getPrintableDeviceIdentifier(msg);
  Serial.print("Device ID: "); Serial.println(msg);
  DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
  Serial.print("Unique ID: "); Serial.println(msg);
  DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
  Serial.print("Network ID & Device Address: "); Serial.println(msg);
  DW1000Ng::getPrintableDeviceMode(msg);
  Serial.print("Device mode: "); Serial.println(msg);
  // attach callback for (successfully) sent and received messages
  DW1000Ng::attachSentHandler(handleSent);
  DW1000Ng::attachReceivedHandler(handleReceived);
  // anchor starts by transmitting a POLL message
}

void handleSent() {
  // status change on sent success
  sentAck = true;
}

void handleReceived() {
  // status change on received success
  receivedAck = true;
}


void loop() {
  
  for (int i = 0; i < 10; i++) {
    data[18] = master_address;
    data[19] = i + 1;

    DW1000Ng::setTransmitData(data, LEN_DATA);
    DW1000Ng::startTransmit();
    while (!sentAck) {

    };

    sentAck = false;
    delay(50);

  }
}