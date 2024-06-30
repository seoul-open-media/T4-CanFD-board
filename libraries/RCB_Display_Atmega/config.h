#ifndef ROBOT_CONTROL_BOARD_DISPLAY_CONFIG_H
#define ROBOT_CONTROL_BOARD_DISPLAY_CONFIG_H

#define I2C_ADDRESS  0x3C
#define RST_PIN      -1       // Reset pin # (or -1 if sharing Arduino reset pin)

#include "../version.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

SSD1306AsciiAvrI2c oled;

#endif