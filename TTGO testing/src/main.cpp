#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include "utilities.h"
#include <TinyGPS++.h>
#include "barometer_wrapper.h"

//https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/ArduinoLoRa/LoRaSender/LoRaSender.ino

int syncWord = 0xF3;
byte _localAddress = 0xFF; // address of this device
byte _destination = 0xFF;  // destination to send to

TinyGPSPlus gps;
void displayInfo();

//lora message function (this can be read by the current LoPy implementation)
void sendMessage(String outgoing, int lora_message_id)
{
	LoRa.beginPacket();			   // start packet
	LoRa.write(_destination);	   // add destination address
	LoRa.write(_localAddress);	   // add sender address
	LoRa.write(lora_message_id);   // add message ID
	LoRa.write(outgoing.length()); // add payload length
	LoRa.print(outgoing);		   // add payload
	LoRa.endPacket();			   // finish packet and send it
}

void setup()
{
	Serial.begin(115200);
	Serial.println("TTGO test");

	initBoard();
	Wire.begin(I2C_SDA, I2C_SCL);
	delay(1500);

	barometer::setup();

	SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
	LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);

	if (!LoRa.begin(433E6))
	{
		Serial.println("Starting LoRa failed!");
		while (1)
			;
	}

	//setting paramaters
	LoRa.setSyncWord(0xF3);
	LoRa.setTxPower(14);
	LoRa.setSpreadingFactor(10);
	LoRa.setCodingRate4(6);
	LoRa.setSignalBandwidth(62.5E3);

	Serial.println("Lora initialized!");
}

void loop()
{
	//*gps
	while (Serial1.available() > 0)
		if (gps.encode(Serial1.read()))
			displayInfo();

	if (millis() > 5000 && gps.charsProcessed() < 10)
	{
		Serial.println(F("No GPS detected: check wiring."));
		while (true)
			;
	}

	//*barometer
	barometer::readSensor();

	//*LORA
	//sendMessage("Hello", 0);
	//delay(1000);
	//Serial.println("Sent");
}

void displayInfo()
{
	Serial.print(F("Location: "));
	if (gps.location.isValid())
	{
		Serial.print(gps.location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gps.location.lng(), 6);
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F("  Date/Time: "));
	if (gps.date.isValid())
	{
		Serial.print(gps.date.month());
		Serial.print(F("/"));
		Serial.print(gps.date.day());
		Serial.print(F("/"));
		Serial.print(gps.date.year());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.print(F(" "));
	if (gps.time.isValid())
	{
		if (gps.time.hour() < 10)
			Serial.print(F("0"));
		Serial.print(gps.time.hour());
		Serial.print(F(":"));
		if (gps.time.minute() < 10)
			Serial.print(F("0"));
		Serial.print(gps.time.minute());
		Serial.print(F(":"));
		if (gps.time.second() < 10)
			Serial.print(F("0"));
		Serial.print(gps.time.second());
		Serial.print(F("."));
		if (gps.time.centisecond() < 10)
			Serial.print(F("0"));
		Serial.print(gps.time.centisecond());
	}
	else
	{
		Serial.print(F("INVALID"));
	}

	Serial.println();
}