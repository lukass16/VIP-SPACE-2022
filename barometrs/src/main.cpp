#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <LoRa.h>
#include <heltec.h>
#include <WiFi.h>
#include "barometer_wrapper.h"

//some links used
//https://github.com/m1k0s/ESP32-HelloWorld3D/blob/master/include/display.h
//https://randomnerdtutorials.com/esp32-web-server-arduino-ide/

//define oled object
U8X8_SSD1306_128X64_NONAME_SW_I2C oled(15, 4, 16);


//definitions for LoRa
#define SS 18
#define RST 14
#define DI0 26

int syncWord = 0xF3;
byte _localAddress = 0xFF; // address of this device
byte _destination = 0xFF;  // destination to send to


//*definitions for wifi
const char *ssid = "Tele2-8c311";
const char *password = "8g6ihj47rb2";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
//*wifi end

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
	Serial.println("Heltec test");

	Wire.begin(21, 22);
	barometer::setup();

	//oled setup
	oled.begin();
	oled.setFont(u8x8_font_chroma48medium8_r);
	oled.drawString(1, 3, "Shalom");

	//Lora
	SPI.begin(5, 19, 27, 18);
	LoRa.setPins(SS, RST, DI0);
	if (!LoRa.begin(433E6))
	{
		Serial.println("Starting LoRa failed!");
		while (1);
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
	barometer::readSensor();
}
