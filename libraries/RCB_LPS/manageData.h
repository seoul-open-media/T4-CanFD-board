#ifndef ROBOT_CONTROL_BOARD_LPS_MANAGEDATA_H
#define ROBOT_CONTROL_BOARD_LPS_MANAGEDATA_H

#include <Audio.h>
#include <Smoothed.h>
#include "hwSerial.h"
#include "containers.h"
#include "positioning.h"
#include "display.h"

// Read distances data ATMega328P sent (23 bytes) from Serial1
void readDistancesData()
{
    if (HWSERIAL.available() > 22)
    {
        // Read verifier signals
        byte first_byte = HWSERIAL.read();
        byte second_byte = HWSERIAL.read();

        // Read distances from anchors
        if (first_byte == 255 && second_byte == FINAL_RESULT)
        {
            for (int i = 0; i < 20; i++)
            {
                byte read = HWSERIAL.read();
                if (1 < read && read < 250) // value <=1 or >= 250 means that it is an error flag sent from the ATMega
                {
                    distancesToAnchors[i] = 0.1 * ((float)read); // * 0.1 to convert to meters
                }
            }

            // Read elapsed time
            elapsedTime = HWSERIAL.read(); // s_data[22] from ATMega328P, in milliseconds

            hwserialDataReceived = true;
        }
        else
        {
            while (HWSERIAL.available())
            {
                HWSERIAL.read();
            }
            HWSERIAL.clear();
        }
    }
}

void runLps()
{
    AudioNoInterrupts();
    readDistancesData();
    AudioInterrupts();

    if (hwserialDataReceived)
    {
        distToAnchorSmoothed1.add(distancesToAnchors[0]);
        distToAnchorSmoothed2.add(distancesToAnchors[1]);
        distToAnchorSmoothed3.add(distancesToAnchors[2]);

        getLpsXyz();
        hwserialDataReceived = false;
    }
}

#endif