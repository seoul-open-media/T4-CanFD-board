#ifndef ROBOT_CONTROL_BOARD_DISPLAY_FUNCTIONS_H
#define ROBOT_CONTROL_BOARD_DISPLAY_FUNCTIONS_H

#include "config.h"

void initDisplay()
{
  //  Wire1.setSCL(16);
  //  Wire1.setSDA(17);
    Wire1.begin();
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (1)
            ;
    }

    delay(1000);

    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.cp437(true);                 // Use full 256 char 'Code Page 437' font

    delay(1000);
}

void clearDisplay()
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
}

void displayControlBaordData()
{
    clearDisplay();

    display.print("RobotCtrlBoard v.");
    display.println(VERSION);
    display.println();
    display.print("Software v.");
    display.println(SOFT_VERSION);
    display.println();
    display.display();
}

#endif
