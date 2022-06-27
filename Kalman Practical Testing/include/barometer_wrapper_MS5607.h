#pragma once
#include <Wire.h>
#include <MS5x.h>
//#include "sensor_data.h"

namespace barometer
{
    MS5x MS5607(&Wire);

    double temp = 0;
    double pres = 0;
    double alt = 0;
    double seaLevelPressure = 0;

    //*for testing
    unsigned long start_t = 0; //*for testing
    int counter = 0;

    void setup()
    {

        while (MS5607.connect() > 0)
        { // barometer.connect starts wire and attempts to connect to sensor
            Serial.println(F("Error connecting..."));
            delay(500);
        }
        Serial.println(F("Connected to Sensor"));
        delay(5);

        //*Note potentially 44 ms in setup could be saved if the following code is reconstructed
        while (seaLevelPressure == 0)
        {
            MS5607.checkUpdates();
            if (MS5607.isReady())
            {
                // Calculate predicted seaLevel pressure based off a known altitude in meters
                seaLevelPressure = MS5607.getSeaLevel(0); // this functions also as the sea level setter for altitude calculations
                Serial.println("Sea level pressure set as: " + String(seaLevelPressure));
            }
        }
    }

    void readSensor(bool temperatureCorrected = false)
    {
        MS5607.checkUpdates();
        if (MS5607.isReady())
        {
            temp = MS5607.GetTemp();                        // Returns temperature in C
            pres = MS5607.GetPres();                        // Returns pressure in Pascals
            alt = MS5607.getAltitude(temperatureCorrected); // Returns altitude in m - //*optionally temperature corrected
        }
    }

    void printAll()
    {
        Serial.print(F("The Temperature is: "));
        Serial.print(temp);
        Serial.print(F("\tThe Pressure is: "));
        Serial.print(pres);
        Serial.print(F("\tThe Altitude is: "));
        Serial.println(alt);
    }

    float getVertVelocity() // todo make this
    {
        return 42.0;
    }

    double getSensorVariance()
    {
        int it_count = 500;
        double avg = 0, sumsq = 0;
        double Z[it_count];

        // get measurement uncertainty (R)
        for (int i = 0; i < it_count; i++) // get readings and average
        {
            barometer::readSensor();
            Z[i] = barometer::alt;
            avg += barometer::alt;
            delay(50);
        }

        avg = avg / it_count;

        // calculate variance
        for (int i = 0; i < it_count; i++)
        {
            sumsq += (Z[i] - avg) * (Z[i] - avg);
        }

        Serial.println("Variance: " + String(sumsq / (it_count - 1), 7));
        return sumsq / (it_count - 1);
    }

}