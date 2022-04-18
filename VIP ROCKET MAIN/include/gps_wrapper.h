#pragma once
#include <TinyGPS++.h>
#include "sensor_data.h"
#include "EEPROM.h"

#define RXPIN 16  //(important to remember about crossover connection)
#define TXPIN 17 // 

namespace gps
{
    TinyGPSPlus gps;
    sens_data::GpsData lastData; // Last data so that values of zero don't get sent when gps doesn't have lock on
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
/*
        if (hasData)
        {
            Serial.print("LAT=");
            Serial.println(gps.location.lat(), 6);
            Serial.print("LONG=");
            Serial.println(gps.location.lng(), 6);
            Serial.print("ALT=");
            Serial.println(gps.altitude.meters(), 1);
            Serial.println();
        }
*/
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

        sens_data::GpsData getGpsState()
        {
            sens_data::GpsData gd;
            if(gps.location.isValid())
            {
                //adding last good values
                lastData.lat = lastLatitude();
                lastData.lng = lastLongitude();
                lastData.alt = lastAltitude();
                gd.lat = lastLatitude();
                gd.lng = lastLongitude();
                gd.alt = lastAltitude();
                gd.sats = getSatellites();
                return gd;
            }
            else
            {
                return lastData;
            }
        }

}