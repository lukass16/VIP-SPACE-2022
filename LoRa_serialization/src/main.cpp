// izmantotā bibliotēka https://github.com/thesolarnomad/lora-serialization
// apkopojums par datu izmēriem un darbu ar datiem https://docs.google.com/document/d/18rY7bKcYi04HzuuFggZDrSUo8SFe7OQvEMxzggwogG4/edit

#include <Arduino.h>
#include "LoraMessage.cpp"
#include "LoraMessage.h"
#include <lora_wrapper.h>
#include <SPI.h>
#include <heltec.h>
#include <LoraEncoder.cpp>
#include <LoraEncoder.h>
#include <LoRa.h>
    float x = 100.11, y = 200.22, z = 300.33, acc_x = 90.9, acc_y = 80.8, acc_z = 70.7; // Megnetometer data simulator
    double lat = 56.3859, lng = 29.9254, gps_alt = 1500;                                // GPS data simulator
    float temperature = 20.5, pressure=10000, bar_alt = 1600, vert_velocity = 38;       // Barometer data simulator
    float bat1 = 7.9, bat2 = 9.9;                                                       // Battery data simulator
    int sats = 10, bs = 1000;

    // storage space for data
    byte mydata[35]; // 35 = number of bytes to store
    // adding as an object ... lkm xD
    LoraEncoder encoder(mydata);

void setup()
{
    Serial.begin(115200);
    lora::setup();
    Serial.println("Lora initialized!");

//......................................Ja gribas sensorus atsevišķi ierakstīt buferē.......................
//* vienīgais nav decoderis šiem encoders – bet varu uztaisīt
    //encoder.writeHumidity(100.11);
    // encoder.writeGPS(lat, lng, alt, sats);          // lat, lng = each 32 bits = 8 bytes
    //                                                 // alt = 16 bits  = 2 bytes
    //                                                 // sats= 8 bits = 1 byte
    //                                                 //  8+2+1 -> 11 bytes total
    // encoder.writeMag(x, y, z, acc_x, acc_y, acc_z); // each 16 bits = 2 bytes -> 2*6 = 12 bytes total

    //encoding data
    encoder.writeMessage(x, y, z, acc_x, acc_y, acc_z,lat, lng, gps_alt, sats,temperature, pressure, bar_alt, vert_velocity, bat1, bat2);
    //deencoding data
    encoder.decodeMessage(mydata); // writes decoded data on the serial monitor



//........................................write out the contents of the buffer..........................
/*
    for (int i = 0; i <= 35; i++)
        { 
            Serial.println(mydata[i]);
        }   
*/


}
void loop()
{
    
//....................................... write out the contents of the buffer to LoRa....................................
    LoRa.beginPacket();
    for (int i = 0; i <= 23; i++)
    { 
        LoRa.write(mydata[i]);
    }
    LoRa.endPacket();

    Serial.print("Sending packet: ");
    Serial.println(1);

    delay(5000);
}
