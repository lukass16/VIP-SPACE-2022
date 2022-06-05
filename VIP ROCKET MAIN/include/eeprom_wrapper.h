#pragma once
#include "EEPROM.h"

#define EEPROM_SIZE 255

/*
Wrapper designated to write and retrieve data to eeprom
EEPROM is interfaced via writing floats, every 4 EEPROM indexes

Adress list:
*40 - flash lock
*/

namespace eeprom
{
    void setup()
    {
        EEPROM.begin(EEPROM_SIZE);
        Serial.println("EEPROM has been setup");
    }

    //flash interfacing
    void lockFlash()
    {
        Serial.println("Flash locked");
        EEPROM.writeFloat(40, 5); //5 chosen as arbitrary value
        EEPROM.commit();
    }

    void unlockFlash()
    {
        Serial.println("Flash unlocked");
        EEPROM.writeFloat(40, 0);
        EEPROM.commit();
    }

    bool lockedFlash()
    {
        return EEPROM.readFloat(40) == 5;
    }

};