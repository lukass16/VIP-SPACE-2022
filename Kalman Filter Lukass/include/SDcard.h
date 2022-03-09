#pragma once

#include <heltec.h>
#include <mySD.h>

#define _miso 4
#define _mosi 13
#define _sclk 14
#define SD_cs 25

Files myFile;
Files stateFile;
const char* stateFileName = "/filteredScaleQ3.txt";
bool firstWriteStateFile = 1;

namespace SDcard
{

    void setup()
    {
        pinMode(SD_cs, OUTPUT); //SS

        if (SD.begin(SD_cs, _mosi, _miso, _sclk)) {
            Serial.println("SD card initialized OK...");}
        else{
            Serial.println("SD card initialization FAILED...");}

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
        myFile = SD.open("/altitude3.txt"); // Re-open the file for reading:
    }

    void closeFile()
    {
        myFile.close(); // close the file:
    }

    void openStateFile()
    {
        stateFile = SD.open(stateFileName, FILE_WRITES); // Re-open the file for reading:
    }

    void closeStateFile()
    {
        stateFile.close();
    }

    int fileAvailable()
    {
        return myFile.available();
    }

    void saveStateToFile(float p, float v, float a)
    {
        //stateFile = SD.open(stateFileName, FILE_APPEND); // Re-open the file for reading:
        char buffer[40] = "";
        String state = String(p, 2) + "," + String(v, 2) + "," + String(a, 2);
        state.toCharArray(buffer, 40);
        stateFile.println(buffer);
        //stateFile.close(); // close the file:
    }

}