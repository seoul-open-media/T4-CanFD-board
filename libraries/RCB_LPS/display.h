#ifndef ROBOT_CONTROL_BOARD_LPS_DISPLAY_H
#define ROBOT_CONTROL_BOARD_LPS_DISPLAY_H

#include "../RCB_Display/RCB_Display.h"
#include "config.h"
#include "containers.h"

void displayLpsDistances()
{
    clearDisplay();

    display.println("Robot Ctrl Board");
#ifdef MY_ADDRESS
    display.print("Address: ");
    display.println(MY_ADDRESS);
#else
    display.println("Address is undefined");
#endif
    display.println("Dist from Anchors:");
    for (int i = 0; i < 3; i++)
    {
        float distance = distancesToAnchors[i];
        if (distance < maxDistance[i] && distance > minDistance[i])
        {
            display.print(distance);
        }
        else
        {
            display.print("ERR.");
        }

        display.print(", ");
    }
    display.println();
    display.println();
    display.println();
    display.println();
    display.print("elapsed time:");
    display.print(elapsedTime);
    display.print(" ms");

    display.display();
}

void displayLpsDistancesUnconstrained() // Same as displayLpsDistances() but does not check if the distances are within the min and max range
{
    clearDisplay();

    display.println("Robot Ctrl Board");
#ifdef MY_ADDRESS
    display.print("Address: ");
    display.println(MY_ADDRESS);
#else
    display.println("Address is undefined");
#endif
    display.println("Dist from Anchors:");
    for (int i = 0; i < 3; i++)
    {
        display.print(distancesToAnchors[i]);
        display.print(", ");
    }
    display.println();
    display.println();
    display.println();
    display.println();
    display.print("elapsed time:");
    display.print(elapsedTime);
    display.println(" ms");

    display.display();
}

void displayLpsXyz()
{
    clearDisplay();

    display.println("Robot Ctrl Board");
#ifdef MY_ADDRESS
    display.print("Address: ");
    display.println(MY_ADDRESS);
#else
    display.println("Address is undefined");
#endif
    display.println("Dist from Anchors:");
    for (int i = 0; i < 3; i++)
    {
        display.print(distancesToAnchors[i]);
        display.print(", ");
    }
    display.println();
    display.print("X = ");
    display.println(lpsX);
    display.print("Y = ");
    display.println(lpsY);
    display.print("Z = ");
    display.println(lpsZ);
    display.print("elapsed time:");
    display.print(elapsedTime);
    display.print(" ms");

    display.display();
}

#endif
