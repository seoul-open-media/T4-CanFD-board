#ifndef ROBOT_CONTROL_BOARD_DISPLAY_CONFIG_H
#define ROBOT_CONTROL_BOARD_DISPLAY_CONFIG_H

#define SCREEN_ADDRESS  0x3C
#define OLED_RESET      4       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH    128     // OLED display width, in pixels
#define SCREEN_HEIGHT   64      // OLED display height, in pixels

#include "../version.h"
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire2, OLED_RESET);

#endif
