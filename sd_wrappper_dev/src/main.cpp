#include <Arduino.h>
#include <SPI.h>
#include <mySD.h>


#define _miso 4
#define _mosi 13
#define _sclk 14
#define SD_cs 25

#define SDMISO 4 //was 12
#define SDMOSI 13 //!the same
#define SDSCLK 14 //was 2
#define SDCS 25 //!the same

SD_File myFile;

void setup()
{
	Serial.begin(115200);
	Serial.print("Starting SD test");
	//setup
	pinMode(SDCS, OUTPUT); //SS
	if (SD.begin(SDCS, SDMOSI, SDMISO, SDSCLK))
	{
		Serial.println("SD card initialized.");
	}
	else
	{
		Serial.println("SD card initialization FAILED...");
	}

	myFile = SD.open("/test3.txt", F_CREAT);
	myFile.println("testing 1, 2, 3.");
	myFile.println("testing 4, 5, 6.");

	myFile.close();

	//Reading

	myFile = SD.open("/test3.txt");

	String text;
	while (myFile.available())
	{
		char c = myFile.read();
		text += (char)c;
	}
	Serial.println(text);

	myFile.close();

	Serial.println("END");

	//test readline

	myFile = SD.open("/test3.txt");

	String line = "";
	if (myFile.available())
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
		Serial.println(line);
	}
	else
	{
		String err = "ERROR: File not open";
		Serial.println(err);
	}
}

void loop() {}
