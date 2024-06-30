#ifndef ROBOT_CONTROL_BOARD_LPS_I2C0_H
#define ROBOT_CONTROL_BOARD_LPS_I2C0_H

#include "config.h"
#include "containers.h"

#define HWSERIAL Serial1 // Teensy will communicate with ATMega328P (which receives distance data from the anchors) through I2C0 (Serial1)

void initHWSERIAL()
{
  HWSERIAL.begin(58824);
  delay(1000);
  while (!HWSERIAL)
    ;
  Serial.println("HWSERIAL begin");

  byte lenData = 6;
  byte s_data[lenData] = {255, 0, 0, 0, 0, 0};
  s_data[1] = SET_DEFAULT_VALUE;
#ifdef MY_ADDRESS
  s_data[2] = MY_ADDRESS;
#else
  Serial.println("Address is undefined");
#endif
  s_data[3] = NETWORK_ID;
  s_data[4] = TOTAL_NUM_ANCHORS;
  s_data[5] = POLL_ACK_CHECK_THRESHOLD;

  // Send initial data from Teensy to ATMega328P so it can configure itself
  HWSERIAL.write(s_data, lenData);
}

#endif