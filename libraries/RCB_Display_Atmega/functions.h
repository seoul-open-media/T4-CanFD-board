#ifndef ROBOT_CONTROL_BOARD_DISPLAY_FUNCTIONS_H
#define ROBOT_CONTROL_BOARD_DISPLAY_FUNCTIONS_H

#include "config.h"

void initDisplay()
{
    #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
    #else // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
    #endif // RST_PIN >= 0
    // Call oled.setI2cClock(frequency) to change from the default frequency.
    
    oled.setFont(Adafruit5x7);
}

void displayControlBaordData()
{
    uint32_t m = micros();
    oled.clear();
    oled.println("Hello world!");
    oled.println("A long line may be truncated");
    oled.println();
    oled.set2X();
    oled.println("2X demo");
    oled.set1X();
    oled.print("\nmicros: ");
    oled.print(micros() - m);
}

#endif