#pragma once
#include <SPIFFS.h>
#include <mySD.h>

// http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/
// Slikts nosakums, bet šis ir HELTEC
// iedvesma:	default esp32 SD sample code no libary ESP-FTP-Server-Lib
//			https://github.com/LilyGO/TTGO-T-Beam/issues/9

// thread par LoRa un SD konfliktu: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

#define _MISO 13
#define _MOSI 12
#define _SCLK 2
#define _SD_CS 25

namespace SDcard
{

	void setup()
	{
		pinMode(_SD_CS, OUTPUT);

		if (SD.begin(_SD_CS, _MOSI, _MISO, _SCLK))
		{
			Serial.println("SD card initialized OK");
		}
		else
		{
			Serial.println("SD card initialization FAILED");
		}
	}

	SD_File openFile() //returnds file handle
	{
		SD_File fileSD = SD.open("data2.txt", FILE_WRITES);
		return fileSD;
	}

	void closeFile(SD_File fileSD)
	{
		fileSD.close();
	}

	void markPreparation(SD_File fileSD)
	{
		fileSD.println("PREPARATION STATE");
	}

	void markDrogue(SD_File fileSD)
	{
		fileSD.println("DROGUE STATE");
	}

	void markMain(SD_File fileSD)
	{
		fileSD.println("MAIN STATE");
	}

	void markDescent(SD_File fileSD)
	{
		fileSD.println("DESCENT STATE");
	}

	void readFullSD(SD_File rfile)
	{
		if (rfile)
		{
			while (rfile.available())
			{
				Serial.write(rfile.read());
			}
		}
		else
		{
			Serial.println("Cannot read SD file");
		}
		rfile.close();
	}

	void test(SD_File fileSD)
	{
		fileSD.println("TESTING SD CARD");
		Serial.println("Wrote to SD: TESTING SD CARD");
	}
	
};