#ifndef ROBOT_CONTROL_BOARD_SD_INIT_H
#define ROBOT_CONTROL_BOARD_SD_INIT_H

#include <SPI.h>
#include <SD.h>

#include "config.h"

File myFile;

void initSdCard()
{
    // Setup SD Card Pins
    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    while (!(SD.begin(SDCARD_CS_PIN)))
    {
        Serial.println("Unable to access the SD card");
        delay(500);
    }

    Serial.println("SD OK!");
}

void writeSDCard()
{
    // open the file. 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
	// close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void readSDCard()
{
    // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
  	// if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

#endif