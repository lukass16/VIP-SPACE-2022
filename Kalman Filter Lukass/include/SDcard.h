#pragma once

#include <heltec.h>
#include <SPIFFS.h>
#include <SD.h>

#define _miso 4
#define _mosi 13
#define _sclk 14
#define SD_cs 25

File myFile;
File stateFile;

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
        else Serial.println("SD connected");
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

    void openFile()
    {
        myFile = SD.open("/altitude.txt"); // Re-open the file for reading:
    }

    void closeFile()
    {
        myFile.close(); // close the file:
    }

    void saveStateToFile(float p, float v, float a)
    {
        stateFile = SD.open("/filtered.txt", FILE_APPEND); // Re-open the file for reading:
        char buffer[40] = "";
        String state = String(p, 2) + "," + String(v, 2) + "," + String(a, 2);
        state.toCharArray(buffer, 40);
        stateFile.println(buffer);
        stateFile.close(); // close the file:
    }

}