#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioPlaySdWav playWav1;
AudioOutputI2S audioOutput;

AudioConnection patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000 sgtl5000_1;

#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 11
#define SDCARD_SCK_PIN 13

void setup()
{
    Serial.begin(9600);

    // Audio connections require memory to work.  For more
    // detailed information, see the MemoryAndCpuUsage example
    AudioMemory(8);

    // Comment these out if not using the audio adaptor board.
    // This may wait forever if the SDA & SCL pins lack
    // pullup resistors
    sgtl5000_1.enable();
    sgtl5000_1.volume(0.5);

    SPI.setMOSI(SDCARD_MOSI_PIN);
    SPI.setSCK(SDCARD_SCK_PIN);
    if (!(SD.begin(SDCARD_CS_PIN)))
    {
        // stop here, but print a message repetitively
        while (1)
        {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }
}

void playFile(const char *filename)
{
    Serial.print("Playing file: ");
    Serial.println(filename);

    // Start playing the file. This sketch continues to
    // run while the file plays.
    playWav1.play(filename);

    // A brief delay for the library read WAV info
    delay(25);

    // Simply wait for the file to finish playing.
    while (playWav1.isPlaying())
    {
        // Serial.println(playWav1.positionMillis());
    }
}

void loop()
{
    playFile("SDTEST1.WAV"); // filenames are always uppercase 8.3 format
    Serial.println("Intermission");
    delay(500);
    playFile("SDTEST2.WAV");
    Serial.println("Intermission");
    delay(500);
    playFile("SDTEST3.WAV");
    Serial.println("Intermission");
    delay(500);
    playFile("SDTEST4.WAV");
    Serial.println("All files played!");
    delay(1500);
}
