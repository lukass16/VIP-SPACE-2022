#pragma once
#include <LoRa.h>
#include <SPI.h>
#include <lcd_rswrapper.h>
#include <LoraEncoder.h>
#include "LoraMessage.h"
#include "sensor_data.h"

// https://randomnerdtutorials.com/ttgo-lora32-sx1276-arduino-ide/
// https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

#define B_SIZE 37

namespace lora
{

    // storage space for data
    byte message[B_SIZE]; // main buffer

    LoraEncoder encoder(message);

    sens_data::SensorData s_data; // data object

    String outgoing; // outgoing message

    // defining received message buffer
    char received[49];

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

    /*
        void readMessage() {
            onReceive(LoRa.parsePacket());
        }
    */
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
        String message = "";
        if (packetSize)
        {
            Serial.println("Received message: " + String(packetSize) + " ");
            // read packet header bytes:
            int recipient = LoRa.read();       // recipient address
            byte sender = LoRa.read();         // sender address
            byte incomingMsgId = LoRa.read();  // incoming msg ID
            byte incomingLength = LoRa.read(); // incoming msg length

            if (recipient != _localAddress && recipient != 0xBB)
            {
                Serial.print("This message is not for me, recipient: " + String(recipient) + " ");
                return "NULL";
            }
            while (LoRa.available())
            {
                message += (char)LoRa.read();
            }
            Serial.print(message);
        }
        else
        {
            return "NULL";
        }
        Serial.println(" RSSI: " + String(LoRa.packetRssi()));
        return message;
    }

    sens_data::SensorData readEncodedMessage()
    {
        int packetSize = LoRa.parsePacket();

        if (packetSize)
        {
            Serial.print("Received packet: " + String(packetSize) + " ");

            // read packet header bytes:
            int recipient = LoRa.read();       // recipient address
            byte sender = LoRa.read();         // sender address
            byte incomingMsgId = LoRa.read();  // incoming msg ID
            byte incomingLength = LoRa.read(); // incoming msg length

            int i = 0;
            if (recipient != _localAddress && recipient != 0xBB)
            {
                s_data.counter = -2;
                Serial.println("This message is not for me, recipient: " + String(recipient));
                return s_data;
            }
            while (LoRa.available())
            {
                byte read = LoRa.read();
                message[i] = read;
                // Serial.print(read);
                i++;
            }
            s_data = encoder.decodeMessage(message); // also prints out decoded variables in a comma separated format
        }
        else
        {
            s_data.counter = -1;
            return s_data;
        }
        Serial.println(" RSSI: " + String(LoRa.packetRssi()));
        return s_data;
    }

}