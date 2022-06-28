#include <SPIFFS.h>
#include <mySD.h>
// http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/
// Slikts nosakums, bet šis ir HELTEC
// iedvesma:	default esp32 SD sample code no libary ESP-FTP-Server-Lib
//			https://github.com/LilyGO/TTGO-T-Beam/issues/9

// thread par LoRa un SD konfliktu: https://stackoverflow.com/questions/57454066/how-to-use-2-spi-devices-lora-and-sd-card-on-esp32

#define _miso 13
#define _mosi 12
#define _sclk 2
#define SD_cs 25

namespace SDCard
{

	void setup()
	{
		pinMode(SD_cs, OUTPUT); // SS

		if (SD.begin(SD_cs, _mosi, _miso, _sclk))
		{
			Serial.println("SD card initialized OK...");
		}
		else
		{
			Serial.println("SD card initialization FAILED...");
		}
	}

	void readSD(SD_File read)
	{
		if (read)
		{
			while (read.available())
			{
				Serial.write(read.read());
			}
		}
		else
		{
			Serial.println("Cannot read SD file");
		}
				
		read.close();
	}
};