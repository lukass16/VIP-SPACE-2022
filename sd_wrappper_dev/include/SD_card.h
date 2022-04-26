#include <SPIFFS.h>
#include <mySD.h>

//https://github.com/nhatuan84/esp32-micro-sdcard

#define SDMISO 4
#define SDMOSI 13
#define SDSCLK 14
#define SDCS 25

namespace SDcard
{
	void setup()
	{
		pinMode(SDCS, OUTPUT); //SS
		if (SD.begin(SDCS, SDMOSI, SDMISO, SDSCLK))
		{
			Serial.println("SD card initialized.");
		}
		else
		{
			Serial.println("SD card initialization FAILED...");
		}
	}

	

	// void readSD(SD_File read)
	// {
	// 	if (read)
	// 		while (read.available())
	// 			Serial.write(read.read());
	// 	read.close();
	// }

	// void DoData(SD_File write, SD_File read)
	// {
	// 	String line = "";

	// 	if (read)
	// 	{
	// 		while (read.available())
	// 		{
	// 			while (read.available())
	// 			{	  // Read from the file until there's nothing else in it:
	// 				{ // Read from the file until a newline character is encountered:
	// 					char c = read.read();
	// 					if (c == ',')
	// 						break;
	// 					line += (char)c;
	// 				}
	// 			}
	// 			write.println((line.toDouble()));
	// 			Serial.println(line);
	// 			line = "";
	// 		}
	// 	}
	// }
}
