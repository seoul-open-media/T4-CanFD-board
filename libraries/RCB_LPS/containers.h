#ifndef ROBOT_CONTROL_BOARD_LPS_CONTAINERS_H
#define ROBOT_CONTROL_BOARD_LPS_CONTAINERS_H

#include <Smoothed.h>

// xyz coordinates of this tag
float lpsX, lpsY, lpsZ;

// Container for the data sent from the DWM1000 through Serial1
float distancesToAnchors[20];         // Distance from this tag to the anchors
byte elapsedTime;                     // Elapsed time
boolean hwserialDataReceived = false; // Flag indicating that new data were successfully retrieved into distancesToAnchors[]

Smoothed<float> distToAnchorSmoothed1, distToAnchorSmoothed2, distToAnchorSmoothed3;

void initLps()
{
    distToAnchorSmoothed1.begin(SMOOTHED_AVERAGE, 5);
    distToAnchorSmoothed2.begin(SMOOTHED_AVERAGE, 5);
    distToAnchorSmoothed3.begin(SMOOTHED_AVERAGE, 5);
}

#endif