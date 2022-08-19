#pragma once
#include <Wire.h>
#include <MS5x.h>

namespace barometer
{
    MS5x MS5607(&Wire);

    double temp = 0;
    double pres = 0;
    double alt = 0;
    double seaLevelPressure = 101667.00;
    double sampledSeaLevelPressure = 0;

    int sampleTimes = 10; // the amount of times the pressure is sampled
    int sampleDelay = 50; // the delay between samples [ms]

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

        // setting sea level pressure
        MS5607.setSeaLevel(seaLevelPressure);
    }

    float sampleSeaLevel() // using function from library to sample sea level pressure, sets the current altitude as 0
    {
        start_t = millis();
        while (sampledSeaLevelPressure == 0)
        {
            counter++;
            MS5607.checkUpdates();
            if (MS5607.isReady())
            {
                // Calculate predicted seaLevel pressure based off a known altitude in meters
                sampledSeaLevelPressure = MS5607.getSeaLevel(0.0); // this functions also as the sea level setter for altitude calculations
                Serial.println("Sea level pressure set as: " + String(sampledSeaLevelPressure));
                Serial.println("Time ellapsed while sampled: " + String(millis() - start_t, 5)); 
                Serial.println("Cycles ellapsed while sampled: " + String(counter));
            }
        }
        return sampledSeaLevelPressure;
    }

    float manualSampleSeaLevel() //manual function for getting sea level pressure by averaging readings
    {
        float divider = sampleTimes;
        float pressureSum = 0;
        for (int i = 0; i < sampleTimes; i++)
        {
            MS5607.checkUpdates();
            if (MS5607.isReady())
            {
                pres = MS5607.GetPres();
                if (pres != 0)
                {
                    pressureSum += pres;
                }
                else
                {
                    divider -= 1;
                }
                delay(sampleDelay);
                Serial.println(i);
            }
        }
        seaLevelPressure = pressureSum / divider; // calculate average seaLevelPressure
        return seaLevelPressure;
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
}