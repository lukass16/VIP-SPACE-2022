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
String stateFileName = "/mfilteredScaleQ2.txt";
bool firstWriteStateFile = 1;

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
        myFile = SD.open("/altitude2.txt"); // Re-open the file for reading:
    }

    void closeFile()
    {
        myFile.close(); // close the file:
    }

    void openStateFile()
    {
        stateFile = SD.open(stateFileName, FILE_WRITE); // Re-open the file for reading:
    }

    void closeStateFile()
    {
        stateFile.close();
    }

    int fileAvailable()
    {
        return myFile.available();
    }

    void saveStateToFile(double p)
    {
        char buffer[40] = "";
        String state = String(p, 2);
        state.toCharArray(buffer, 40);
        stateFile.println(buffer);
    }

}