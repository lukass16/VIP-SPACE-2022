#include <Arduino.h>
#include <SPI.h>
#include <mySD.h>

//!Don't change without adding comment or notifying
#define SDMISO 12 
#define SDMOSI 13
#define SDSCLK 2
#define SDCS 25

SD_File myFile;

void setup()
{
	Serial.begin(115200);
	Serial.print("Starting SD test");

	//*Setup
	pinMode(SDCS, OUTPUT); // SS
	if (SD.begin(SDCS, SDMOSI, SDMISO, SDSCLK))
	{
		Serial.println("SD card initialized.");
	}
	else
	{
		Serial.println("SD card initialization FAILED...");
	}

	myFile = SD.open("test3.txt", F_WRITE); //*only one file can be open at a time

	if (myFile)
	{
		Serial.println("File opened.");
	}

	myFile.println("testing 1, 2, 3.");
	myFile.println("testing 4, 5, 6.");

	myFile.close();

	//*Reading
	myFile = SD.open("test3.txt");

	if (myFile)
	{
		Serial.println("File opened for reading.");

		while (myFile.available())
		{
			Serial.write(myFile.read());
		}
	}
	// close the file:
	myFile.close();

	/*
	String text;
	while (myFile.available())
	{
		char c = myFile.read();
		text += (char)c;
	}
	Serial.println(text);

	myFile.close();

	Serial.println("END");

	// test readline

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
	*/
}

void loop() {}
