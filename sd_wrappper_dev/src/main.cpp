#include <Arduino.h>
#include <SPI.h>
#include <mySD.h>

#define SDMISO 4
#define SDMOSI 13
#define SDSCLK 14
#define SDCS 25

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

	myFile = SD.open("/test.txt", 'w');
	myFile.println("testing 1, 2, 3.");
	myFile.println("testing 4, 5, 6.");

	myFile.close();

	//Reading

	myFile = SD.open("/test.txt", 'r');

	while (myFile.available())
	{
		Serial.print(myFile.read());
		delay(100);
	}

	myFile.close();

	Serial.println("END");

	//test readline

	myFile = SD.open("/test.txt", 'r');

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
