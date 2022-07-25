#pragma once
#include <TinyGPS++.h>

#define RXPIN 16  //(important to remember about crossover connection)
#define TXPIN 17 // 

namespace gps
{
    TinyGPSPlus gps;
    boolean hasData = false;

    void setup(uint gpsRate = 9600)
    {
        Serial2.begin(gpsRate); //hardware serial 2 (RX-16, TX-17)
        Serial.println("Init GPS: " + String(gpsRate));
    }

    void readGps()
    {
        hasData = false;

        while (Serial2.available())
        {
            gps.encode(Serial2.read());
            hasData = true;
        }

        if (hasData)
        {
            Serial.print("Lat: ");
            Serial.print(gps.location.lat(), 5);
            Serial.print(" Long: ");
            Serial.print(gps.location.lng(), 5);
            Serial.print(" Alt: ");
            Serial.println(gps.altitude.meters(), 1);
            Serial.println();
        }
    }

    double lastLatitude()
    {
        return gps.location.lat(); 
    }

    double lastLongitude()
    {
        return gps.location.lng();
    }

    double lastAltitude()
    {
        return gps.altitude.meters();
    }

    int getSatellites()
    {
        return gps.satellites.value();
    }

    uint8_t getHour()
    {
        return gps.time.hour();
    }

    uint8_t getMinute()
    {
        return gps.time.minute();
    }

    uint8_t getSecond()
    {
        return gps.time.second();
    }

}