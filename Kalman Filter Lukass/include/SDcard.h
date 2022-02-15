#pragma once

#include <heltec.h>
#include <SPIFFS.h>
#include <SD.h>

#define _miso 4
#define _mosi 13
#define _sclk 14
#define SD_cs 25

File myFile;

namespace SDcard
{

    void setup()
    {
        SPIFFS.begin(true);
        SPI.begin(_sclk, _miso, _mosi, SD_cs);
        if (!SD.begin(SD_cs))
        {
            Serial.println("SD Card Mount Failed");
        }
        else
            Serial.println("SD connected");

        myFile = SD.open("/altitude.txt"); // Re-open the file for reading:
    }

    String readline()
    {
        String line = "";
        if (myFile)
        {
            while (myFile.available())
            { // Read from the file until a newline character is encountered:
                char c = myFile.read();
                if (c == '\n')
                {
                    break;
                }
                line += (char)c;
            }
            return line;
        }
        else
        {
            String err = "ERROR: File not open";
            return err;
        }
    }

    void closeFile()
    {
        myFile.close(); // close the file:
    }

}