/*  Compiled in Arduino 1.8.9 with modification
    Arduino pro promini 3.3V 8Mhz
    Gets order from Master to my_address 1, 2
   Alternates mode between initiator and responder every 1sec.
    Data Structure
   https://docs.google.com/spreadsheets/d/1Cie-AcoIuojLqRC60rg6zebejEXcjnMsB474eh22s5Q/edit?usp=sharing
*/

#include <SPI.h>
#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include <DW1000NgTime.hpp>
#include <DW1000NgConstants.hpp>
//#include <PriUint64.h>

#define my_address  103
#define initiator_mode 1
#define responder_mode 2
#define FINAL_RESULT 2
#define LEN_DATA 24
byte d_data[LEN_DATA];
byte i_data[LEN_DATA];
byte r_data[LEN_DATA];
byte distance_result[20];  // buffer to store ranging data
byte failure_counter[20];
byte _info[80]; // _info[0] song_num _info[1] MSB_duation  _info[2] LSB_duation  _info[3] bow_state

uint8_t network_id = 10;

byte r_destination_address = 1;
byte i_destination_address = 1;

#define master_address 255

// Timeout parameters
#define TIMEOUT_WAIT_ACK_REQ_SENT 5 //5 //ms
#define TIMEOUT_WAIT_ACK 10 //ms
#define TIMEOUT_WAIT_DATA_REPLY_SENT 5 //ms
/*
  // SDS-TWR server states state machine enumeration: see state diagram on documentation for more details
  enum { SDSTWR_ENGINE_STATE_INIT, SDSTWR_ENGINE_STATE_WAIT_START, SDSTWR_ENGINE_STATE_MEMORISE_T2,
       SDSTWR_ENGINE_STATE_SEND_ACK_REQ, SDSTWR_ENGINE_STATE_WAIT_ACK_REQ_SENT, SDSTWR_ENGINE_STATE_MEMORISE_T3,
       SDSTWR_ENGINE_STATE_WAIT_ACK, SDSTWR_ENGINE_STATE_MEMORISE_T6, SDSTWR_ENGINE_STATE_SEND_DATA_REPLY,
       SDSTWR_ENGINE_STATE_WAIT_DATA_REPLY_SENT
     };
*/
#define SDSTWR_ENGINE_STATE_INIT 1
#define SDSTWR_ENGINE_STATE_WAIT_START 2
#define SDSTWR_ENGINE_STATE_MEMORISE_T2 3
#define SDSTWR_ENGINE_STATE_SEND_ACK_REQ 4
#define SDSTWR_ENGINE_STATE_WAIT_ACK_REQ_SENT 5
#define SDSTWR_ENGINE_STATE_MEMORISE_T3 6
#define SDSTWR_ENGINE_STATE_WAIT_ACK 7
#define SDSTWR_ENGINE_STATE_MEMORISE_T6 8
#define SDSTWR_ENGINE_STATE_SEND_DATA_REPLY 9
#define SDSTWR_ENGINE_STATE_WAIT_DATA_REPLY_SENT 10


int32_t lasttimesent = 0;
uint32_t timeout;
int state;

byte my_mode, poll_count;
boolean ok_send_poll, received_poll_ack = false;



// connection pins
const uint8_t PIN_RST = 3; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = 10; // spi select pin

// TODO replace by enum
#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
// message flow state
volatile byte expectedMsgId = POLL_ACK;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
volatile boolean receivedMessage = false;
boolean protocolFailed = false;

// timestamps to remember
uint64_t timePollSent;
uint64_t timePollAckReceived;
uint64_t timeRangeSent;

//responder only
uint64_t timePollReceived;
uint64_t timePollAckSent;
uint64_t timeRangeReceived;

// watchdog and reset period
uint32_t lastActivity , time_poll_sent, time_stamp;


// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 3000;
// ranging counter (per second)
uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;
double distance;

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
  pinMode(13, OUTPUT);
  //Serial.begin(57600);
  //Serial.begin(115200);

  delay(1000);

  //Serial.println(F("### DW1000Ng-arduino-ranging-Initiator ###"));
  // initialize the driver
  DW1000Ng::initialize(PIN_SS, PIN_IRQ, PIN_RST);
  //Serial.println("DW1000Ng initialized ...");
  // general configuration
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

  DW1000Ng::setDeviceAddress(my_address);
  DW1000Ng::setNetworkId(network_id);
  DW1000Ng::setAntennaDelay(16436);

  DW1000Ng::setTXPower(522133279);  // 0x1F1F1F1F
  // //Serial.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed

  delay(1000);
  char msg[128];
  DW1000Ng::getPrintableDeviceIdentifier(msg);
  //Serial.print("Device ID: "); //Serial.println(msg);
  DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
  //Serial.print("Unique ID: "); //Serial.println(msg);
  DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
  //Serial.print("Network ID & Device Address: "); //Serial.println(msg);
  DW1000Ng::getPrintableDeviceMode(msg);
  //Serial.print("Device mode: "); //Serial.println(msg);


  // attach callback for (successfully) sent and received messages
  DW1000Ng::attachSentHandler(handleSent);
  DW1000Ng::attachReceivedHandler(handleReceived);
  // anchor starts by transmitting a POLL message

  initializeDataBuffer();
  state = SDSTWR_ENGINE_STATE_INIT;
}
void initializeDataBuffer() {
  for (int i = 0; i < LEN_DATA; i++) {
    d_data[i] = 0;
    i_data[i] = 0;
    r_data[i] = 0;
  }
  for (int i = 0; i < 80; i++) {
    _info[i] = 0;
  }
  for (int i = 0; i < 20; i++) {
    failure_counter[i] = 0;
  }
}
void noteActivity() {
  // update activity timestamp, so that we do not reach "resetPeriod"
  lastActivity = millis();
}


void resetInactive() {
  // anchor listens for POLL
  expectedMsgId = POLL;
  receiver();
  noteActivity();
  ////Serial.println("resetInactive");
}


void handleSent() {
  // status change on sent success
  sentAck = true;
}

void handleReceived() {
  // status change on received success
  receivedAck = true;
  //receivedMessage = true;
}
void receiver() {
  DW1000Ng::forceTRxOff();
  // so we don't need to restart the receiver manually
  DW1000Ng::startReceive();
  //  //Serial.println("startReceive");
}
void transmitPoll(byte address) {
  i_data[0] = POLL;
  i_data[16] = my_mode; // initiator_mode
  i_data[17] = address;
  i_data[18] = my_address;
  i_data[19] = 0; // _message 0, in case
  DW1000Ng::setTransmitData(i_data, LEN_DATA);
  DW1000Ng::startTransmit();
  // Serial.print("TransmitPoll to"); //Serial.println(address);
}
void transmitRangeReport(float curRange) {
  r_data[0] = RANGE_REPORT;
  r_data[16] = my_mode; // responder_mode
  r_data[17] = r_destination_address;
  r_data[18] = my_address;
  r_data[19] = 0; // _message 0, in case

  // add info
  /////////////////////////////////
  r_data[20] = _info[4 * (my_address - 1)];
  r_data[21] = _info[4 * (my_address - 1) + 1];
  r_data[22] = _info[4 * (my_address - 1) + 2];
  r_data[23] = _info[4 * (my_address - 1) + 3];
  /////////////////////////////////

  // write final ranging result
  memcpy(r_data + 1, &curRange, 4);
  DW1000Ng::setTransmitData(r_data, LEN_DATA);
  DW1000Ng::startTransmit();
  // //Serial.println("transmitRangeReport");
}

void transmitRangeFailed() {
  r_data[0] = RANGE_FAILED;
  r_data[16] = my_mode; // responder_mode
  r_data[17] = r_destination_address;
  r_data[18] = my_address;
  r_data[19] = 0; // _message 0, in case
  DW1000Ng::setTransmitData(r_data, LEN_DATA);
  DW1000Ng::startTransmit();
  ////Serial.println("transmitRangeFailed");
}

void transmitPollAck() {
  r_data[0] = POLL_ACK;
  r_data[16] = my_mode; // responder_mode
  r_data[17] = r_destination_address;
  r_data[18] = my_address;
  r_data[19] = 0; // _message 0, in case
  DW1000Ng::setTransmitData(r_data, LEN_DATA);
  DW1000Ng::startTransmit();
  // //Serial.println("transmitPollAck");
}





void loop() {
  int32_t curMillis = millis();

  switch (state) {

    case SDSTWR_ENGINE_STATE_INIT :
      //Serial.println(1);
      resetInactive();
      state = SDSTWR_ENGINE_STATE_WAIT_START;
      break;

    case SDSTWR_ENGINE_STATE_WAIT_START :
      if (receivedAck) {
        //Serial.println(2);
        receivedAck = false;
        // get message and parse
        DW1000Ng::getReceivedData(r_data, LEN_DATA);
        byte msgId = r_data[0];
        byte mode_from_sender = r_data[16];
        byte to_address = r_data[17];
        byte from_address = r_data[18];
        byte _message = r_data[19];
        r_destination_address = from_address;

        if (to_address != my_address || from_address == master_address) {
          state = SDSTWR_ENGINE_STATE_INIT;
          return;
        }
        
        if (msgId == POLL) {
          state = SDSTWR_ENGINE_STATE_MEMORISE_T2;
          //Serial.println(3);
        } else {
          receiver();
          state = SDSTWR_ENGINE_STATE_WAIT_START;
          //Serial.println(4);
        }
      }
      break;

    case SDSTWR_ENGINE_STATE_MEMORISE_T2 :
      //t2 = decaduino.getLastRxTimestamp();
      timePollReceived = DW1000Ng::getReceiveTimestamp();
      state = SDSTWR_ENGINE_STATE_SEND_ACK_REQ;
      //Serial.println(5);
      break;

    case SDSTWR_ENGINE_STATE_SEND_ACK_REQ :
      //txData[0] = SDSTWR_MSG_TYPE_ACK_REQ;
      expectedMsgId = RANGE;
      //decaduino.pdDataRequest(txData, 1);
      transmitPollAck();


      timeout = millis() + TIMEOUT_WAIT_ACK_REQ_SENT;
      state = SDSTWR_ENGINE_STATE_WAIT_ACK_REQ_SENT;
      //Serial.println(6);
      break;

    case SDSTWR_ENGINE_STATE_WAIT_ACK_REQ_SENT:
      if ( millis() > timeout ) {
        state = SDSTWR_ENGINE_STATE_INIT;
        //Serial.println(7);
      } else {
        if ( sentAck ) {
          timePollAckSent = DW1000Ng::getTransmitTimestamp();
          state = SDSTWR_ENGINE_STATE_MEMORISE_T3;
          sentAck = false;

          //Serial.println("8");
        }
      }
      break;

    case SDSTWR_ENGINE_STATE_MEMORISE_T3 :
      //Serial.println(9);
      // t3 = decaduino.getLastTxTimestamp();
      timeRangeReceived = DW1000Ng::getReceiveTimestamp();
      timeout = millis() + TIMEOUT_WAIT_ACK;
      receiver();
      state = SDSTWR_ENGINE_STATE_WAIT_ACK;
      break;

    case SDSTWR_ENGINE_STATE_WAIT_ACK :
      if ( millis() > timeout) {
        //Serial.println(10);
        state = SDSTWR_ENGINE_STATE_INIT;
      }
      else {
        if (receivedAck) {
          //Serial.println(11);
          receivedAck = false;
          // get message and parse
          DW1000Ng::getReceivedData(r_data, LEN_DATA);
          byte msgId = r_data[0];
          byte mode_from_sender = r_data[16];
          byte to_address = r_data[17];
          byte from_address = r_data[18];
          byte _message = r_data[19];
          r_destination_address = from_address;
          if (msgId == RANGE) {
            state = SDSTWR_ENGINE_STATE_MEMORISE_T6;
          } else {
            //decaduino.plmeRxEnableRequest();
            receiver();
            state = SDSTWR_ENGINE_STATE_WAIT_ACK;
          }
        }
      }
      break;

    case SDSTWR_ENGINE_STATE_MEMORISE_T6 :
      //Serial.println(12);
      timeRangeReceived = DW1000Ng::getReceiveTimestamp();
      timePollSent = DW1000NgUtils::bytesAsValue(r_data + 1, LENGTH_TIMESTAMP);
      timePollAckReceived = DW1000NgUtils::bytesAsValue(r_data + 6, LENGTH_TIMESTAMP);
      timeRangeSent = DW1000NgUtils::bytesAsValue(r_data + 11, LENGTH_TIMESTAMP);
      state = SDSTWR_ENGINE_STATE_SEND_DATA_REPLY;
      break;

    case SDSTWR_ENGINE_STATE_SEND_DATA_REPLY :
      //Serial.println("13");
      expectedMsgId = POLL;

      distance = DW1000NgRanging::computeRangeAsymmetric(timePollSent,
                 timePollReceived,
                 timePollAckSent,
                 timePollAckReceived,
                 timeRangeSent,
                 timeRangeReceived);
      /* Apply simple bias correction */
      distance = DW1000NgRanging::correctRange(distance);

      transmitRangeReport(distance * DISTANCE_OF_RADIO_INV);

      timeout = millis() + TIMEOUT_WAIT_DATA_REPLY_SENT;
      successRangingCount++;
      // update sampling rate (each second)
      if (curMillis - rangingCountPeriod > 1000) {
        samplingRate = (1000.0f * successRangingCount) / (curMillis - rangingCountPeriod);
        rangingCountPeriod = curMillis;
        successRangingCount = 0;
      }
      state = SDSTWR_ENGINE_STATE_WAIT_DATA_REPLY_SENT;
      //Serial.println(14);
      break;

    case SDSTWR_ENGINE_STATE_WAIT_DATA_REPLY_SENT:
      //Serial.println(15);
      if ( (millis() > timeout) || sentAck ) {
        sentAck = false;
        state = SDSTWR_ENGINE_STATE_INIT;
        /*
          Serial.print("PS:"); Serial.println(PriUint64<DEC>(timePollSent));
          Serial.print("PR:"); Serial.println(PriUint64<DEC>(timePollReceived));
          Serial.print("PAS:"); Serial.println(PriUint64<DEC>(timePollAckSent));
          Serial.print("PAR:"); Serial.println(PriUint64<DEC>(timePollAckReceived));
          Serial.print("RS:"); Serial.println(PriUint64<DEC>(timeRangeSent));
          Serial.print("RR:"); Serial.println(PriUint64<DEC>(timeRangeReceived));
          Serial.print("d:");
          Serial.println(distance);
          String rangeString = "Range: "; rangeString += distance; rangeString += " m";
          rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm";
          rangeString += "\t Sampling: "; rangeString += samplingRate; rangeString += " Hz";
          Serial.println(rangeString);
          Serial.print("distance * DISTANCE_OF_RADIO_INV = ");
          Serial.println(distance * DISTANCE_OF_RADIO_INV);
          Serial.print("FP power is [dBm]: "); Serial.print(DW1000Ng::getFirstPathPower());
          Serial.print("RX power is [dBm]: "); Serial.println(DW1000Ng::getReceivePower());
          Serial.print("Receive quality: "); Serial.println(DW1000Ng::getReceiveQuality());
        */
      }
      break;

    default :
      //Serial.println(16);
      state = SDSTWR_ENGINE_STATE_INIT;
      break;

  }


}