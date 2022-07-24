#pragma once
#include <LoRa.h>
#include <SPI.h>
#include <LoraEncoder.h>
#include "LoraMessage.h"

// https://randomnerdtutorials.com/ttgo-lora32-sx1276-arduino-ide/
// https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino

namespace lora
{

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

    // storage space for data
    byte message[38]; // 35 = number of bytes to store
    // adding as an object ... lkm xD
    LoraEncoder encoder(message);

    String outgoing; // outgoing message

    // defining received message buffer
    char received[80];

    int syncWord = 0xF3;

    byte _localAddress = 0xFF; // address of this device
    byte _destination = 0xBB;  // destination to send to
    long lastSendTime = 0;     // last send time
    int interval = 2000;       // interval between sends
    boolean _canPrintHeaderMessage = false;

    void sendMessage(String outgoing, int lora_message_id);
    String onReceive(int packetSize); // added math!
    //*NEW
    int getPacketSize();

    void setup(double frequency = 433E6, boolean canPrintHeaderMessage = false)
    {

        _canPrintHeaderMessage = canPrintHeaderMessage;

        // SPI LoRa pins
        SPI.begin(SCK, MISO, MOSI, SS);
        // setup LoRa transceiver module
        LoRa.setPins(SS, RST, DIO0);

        if (!LoRa.begin(frequency))
        {
            Serial.println("Starting LoRa failed!");
            while (1)
                ;
        }

        // setting paramaters
        LoRa.setSyncWord(0xF3);
        LoRa.setTxPower(10);
        LoRa.setSpreadingFactor(10);
        LoRa.setCodingRate4(6);
        LoRa.setSignalBandwidth(62.5E3);
        Serial.println("Lora connected");
        Serial.println(LoRa.available());
    }

    long freqError()
    {
        return LoRa.packetFrequencyError();
    }

    //! careful
    int getPacketSize()
    {
        return LoRa.parsePacket();
    }

    int getPacketRssi()
    {
        return LoRa.packetRssi();
    }

    float getPacketSNR()
    {
        return LoRa.packetSnr();
    }

    //*NEW
    String readMessage()
    {
        int packetSize = LoRa.parsePacket();
        if (packetSize)
        {
            int i = 0;
            while (LoRa.available())
            {
                byte read = LoRa.read();
                message[i] = read;
                //Serial.print(read);
                i++;
            }
            encoder.decodeMessage(message);
        }
        else
        {
            return "NULL";
        }
        Serial.println(" RSSI: " + String(LoRa.packetRssi()));
        return "<3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Nomaini Šo, luni!";
    }

}