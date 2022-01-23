#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include "utilities.h"

//https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/examples/ArduinoLoRa/LoRaSender/LoRaSender.ino

int syncWord = 0xF3;
byte _localAddress = 0xFF; // address of this device
byte _destination = 0xFF;  // destination to send to


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
	
	SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
	LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);

	if (!LoRa.begin(433E6)) {
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
	sendMessage("Hello", 0);
	delay(1000);
	Serial.println("Sent");
}