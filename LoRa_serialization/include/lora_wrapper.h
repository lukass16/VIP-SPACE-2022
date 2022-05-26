#pragma once
#include <LoRa.h>
#include <SPI.h>

// https://randomnerdtutorials.com/ttgo-lora32-sx1276-arduino-ide/
// https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino

// definitions for LoRa
#define SS 18
#define RST 14
#define DI0 26
#define SCK 5
#define MISO 19
#define MOSI 27

namespace lora
{
    String outgoing; // outgoing message

    int syncWord = 0xF3;
    byte _localAddress = 0xFF; // address of this device
    byte _destination = 0xFF;  // destination to send to

    boolean _canPrintHeaderMessage = false;

    void sendMessage(String outgoing, int lora_message_id);

    void setup(double frequency = 433E6, boolean canPrintHeaderMessage = false)
    {
        _canPrintHeaderMessage = canPrintHeaderMessage;

        // SPI LoRa pins
        SPI.begin(SCK, MISO, MOSI, SS);
      
        // setup LoRa transceiver module
        LoRa.setPins(SS, RST, DI0);

        if (!LoRa.begin(frequency))
        {
            Serial.println("Starting LoRa failed!");
            while (1);
        }

        // setting paramaters
        LoRa.setSyncWord(0xF3);
        LoRa.setTxPower(14);
        LoRa.setSpreadingFactor(10);
        LoRa.setCodingRate4(6);
        LoRa.setSignalBandwidth(62.5E3);

        Serial.println("Lora initialized!");
    }

    void sendMessage(String outgoing, int lora_message_id)
    {
        LoRa.beginPacket();            // start packet
        LoRa.write(_destination);      // add destination address
        LoRa.write(_localAddress);     // add sender address
        LoRa.write(lora_message_id);   // add message ID
        LoRa.write(outgoing.length()); // add payload length
        LoRa.print(outgoing);          // add payload
        LoRa.endPacket();              // finish packet and send it
    }

    int getPacketRssi()
    {
        return LoRa.packetRssi();
    }

}
