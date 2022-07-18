#pragma once
#include <LoRa.h>
#include <SPI.h>
#include "LoraMessage.h"
#include <LoraEncoder.h>
#include "sensor_data.h"
// https://randomnerdtutorials.com/ttgo-lora32-sx1276-arduino-ide/
// https://github.com/sandeepmistry/arduino-LoRa/blob/master/examples/LoRaDuplex/LoRaDuplex.ino

// definitions for LoRa
#define SS 18
#define RST 14
#define DI0 26
#define SCK 5
#define MISO 19
#define MOSI 27

#define B_SIZE 34

namespace lora
{

    //*ENCODING
    byte data[B_SIZE]; // main buffer that is linked with encoder
    LoraEncoder encoder(data);

    void encodeMessage()
    {
        // gather all data
        static int counter = 0;
        sens_data::GpsData gps = s_data.getGpsData();
        sens_data::IMUData imu = s_data.getIMUData();
        sens_data::BarometerData bar = s_data.getBarometerData();
        sens_data::BatteryData bat = s_data.getBatteryData();
        int r_state = s_data.getRocketState();
        // encode
        encoder.writeMessage(gps.lat, gps.lng, gps.alt, gps.sats, imu.acc_x, imu.acc_y, imu.acc_z, bar.pressure, bar.altitude, bar.f_altitude, bar.f_velocity, bat.bat1, r_state, counter);
        
        counter++;
    }

    void sendEncodedMessage()
    {
        LoRa.beginPacket();
        for (int i = 0; i < B_SIZE; i++)
        {
            LoRa.write(data[i]);
        }
        LoRa.endPacket();
    }

    void printBuffer()
    {
        for (int i = 0; i < B_SIZE; i++)
        {
            Serial.print(data[i]);
        }
        Serial.println();
    }

    //*
    
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
            while (1)
                ;
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
