#pragma once
#include <Arduino.h>

#define EEPROMCLEAR 37
#define SW1 38
#define SW2 39
#define PYROSENSE 36
#define FIREPYRO 32


namespace arming
{
    void setup()
    {
        pinMode(EEPROMCLEAR, INPUT);
        pinMode(SW1, INPUT);
        pinMode(SW2, INPUT);
        pinMode(PYROSENSE, INPUT);
        pinMode(FIREPYRO, OUTPUT);

        Serial.println("Arming setup complete!");
    }

    bool clearEEPROM()
    {
        if(digitalRead(EEPROMCLEAR) == 0)
        {
            Serial.println("Clearing EEPROM");
            return 1;
        }
        return 0;
    }
}