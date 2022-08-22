#pragma once
#include "EEPROM.h"

#define EEPROM_SIZE 255

/*
Wrapper designated to write and retrieve data to eeprom
EEPROM is interfaced via writing floats, every 4 EEPROM indexes

Adress list:
*0 - flash lock
*4 - state saving
*8 - sampled pressure saving
*/

namespace eeprom
{
	int prevState = 0;
	float seaLevelPressure = 0;

	void setup()
	{
		EEPROM.begin(EEPROM_SIZE);
		Serial.println("EEPROM has been setup");
	}

	void clean()
	{
        for (int i = 0; i <= 8; i += 4)
        {
            EEPROM.writeFloat(i, 0.0);
        }
        Serial.println("Cleared EEPROM");
        EEPROM.commit();
	}

	// flash interfacing
	void lockFlash()
	{
		EEPROM.writeFloat(0, 1.0); // 1 - Flash locked
		EEPROM.commit();
		Serial.println("Flash locked");
	}

	void unlockFlash()
	{
		EEPROM.writeFloat(0, 0);
		EEPROM.commit();
		Serial.println("Flash unlocked");
	}

	bool lockedFlash()
	{
		bool locked = false;
		if (EEPROM.readFloat(0) == 1)
		{
			locked = true;
		}
		return locked;
	}

	// STATE MARKING
	/*
	1 - Launch has been detected
	2 - Apogee has been detected
	3 - Main ejection has happened
	*/

	void markLaunch()
	{
		Serial.println("Marking launch in EEPROM");
		EEPROM.writeFloat(4, 1.0);
		EEPROM.commit();
	}

	void markApogee()
	{
		Serial.println("Marking apogee in EEPROM");
		EEPROM.writeFloat(4, 2.0);
		EEPROM.commit();
	}

	void markMainEjection()
	{
		Serial.println("Marking main ejection in EEPROM");
		EEPROM.writeFloat(4, 3.0);
		EEPROM.commit();
	}

	int readPreviousState()
	{
		prevState = (int)(EEPROM.readFloat(4));
		Serial.println("Previous state in EEPROM read as: " + String(prevState));
		return prevState;
	}

	bool hasBeenLaunch()
	{
		if(prevState == 1)
		{
			Serial.println("Previous launch detected in EEPROM!");
			return 1;
		}
		return 0;
	}

	bool hasBeenApogee()
	{
		if(prevState == 2)
		{
			Serial.println("Apogee detected in EEPROM!");
			return 1;
		}
		return 0;
	}

	bool hasBeenMainEjection()
	{
		if(prevState == 3)
		{
			Serial.println("Main ejection detected in EEPROM!");
			return 1;
		}
		return 0;
	}

	void writeSampledPressure(float sampledSeaLevelPressure)
	{
		Serial.println("Writing sampled sea level pressure to EEPROM");
		EEPROM.writeFloat(8, sampledSeaLevelPressure);
		EEPROM.commit();
	}

	float readSampledPressure()
	{
		seaLevelPressure = EEPROM.readFloat(8);
		Serial.println("Sea level pressure in EEPROM read as: " + String(seaLevelPressure));
		return seaLevelPressure;
	}

};