#pragma once
#include <Wire.h>
#include <MS5x.h>
#include "sensor_data.h"
#include "kalman.h"
#include "eeprom_wrapper.h"

namespace barometer
{
    MS5x MS5607(&Wire);

    double temp = 0;
    double pres = 0;
    double alt = 0;
    double seaLevelPressure = 101667.00;
    double sampledSeaLevelPressure = 0;

    // Kalman (filtered) readings
    float f_alt = 0;
    float f_vel = 0;
    float f_acc = 0;

    // Abnormal descent speed detection parameter
    float abn_speed = -45.0; // m/s

    unsigned long kalman_t; // Kalman timer
    float t_change = 0;     // change of time from previous prediction to current


    void setup()
    {
        while (MS5607.connect() > 0) // barometer.connect starts wire and attempts to connect to sensor
        { 
            Serial.println(F("Error connecting..."));
            delay(500);
        }
        Serial.println(F("Connected to MS5607"));
        delay(5);

        seaLevelPressure = eeprom::readSampledPressure();

        // setting sea level pressure
        MS5607.setSeaLevel(seaLevelPressure);
        Serial.println("Sea level pressure set to: " + String(seaLevelPressure) + " Pa");

        // kalman setup
        kalman::predict(); // make first prediction
        kalman_t = millis();
    }

    float sampleSeaLevel() // using function from library to sample sea level pressure, sets the current altitude as 0
    {
        while (sampledSeaLevelPressure == 0)
        {
            MS5607.checkUpdates();
            if (MS5607.isReady())
            {
                // Calculate predicted seaLevel pressure based off a known altitude in meters
                sampledSeaLevelPressure = MS5607.getSeaLevel(0.0); // this functions also as the sea level setter for altitude calculations
                Serial.println("Sea level pressure sampled and set as: " + String(sampledSeaLevelPressure));
            }
        }

        eeprom::writeSampledPressure(sampledSeaLevelPressure);

        return sampledSeaLevelPressure;
    }

    void readSensor(bool temperatureCorrected = false)
    {
        MS5607.checkUpdates();
        if (MS5607.isReady())
        {
            temp = MS5607.GetTemp();                        // Returns temperature in C
            pres = MS5607.GetPres();                        // Returns pressure in Pascals //*Note - saved and sent in hPa
            alt = MS5607.getAltitude(temperatureCorrected); // Returns altitude in m //*optionally temperature corrected
        }

        // kalman implementation
        t_change = (millis() - kalman_t) / 1000.0;
        kalman::setDelT(t_change);

        kalman_t = millis();

        kalman::predict();
        kalman::updateBaro(alt);

        // set filtered readings
        f_alt = kalman::getKalmanPosition();
        f_vel = kalman::getKalmanVelocity();
        f_acc = kalman::getKalmanAcceleration();
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

    void printInfoFilteredPosition()
    {
        kalman::printFullInfoPosition(alt);
    }

    void printState()
    {
        kalman::printKalmanState();
    }
 
    //Apogee detection function - currently unused
    bool apogeeDetected(int times = 3, float minimum_height = 20.0) // times - times to detect the vertical speed being below zero, minimum_height - the minimum height at which apogee can be detected, otherwise apogee detection is prohibited
    {
        static int counter = 0;
        if (f_vel < 0 && f_alt > minimum_height)
        {
            counter++;
        }
        else
        {
            counter = 0;
        }
        if (counter > times)
        {
            Serial.println("Apogee detected!");
            return true;
        }
        else
        {
            return false;
        }
    }

    bool mainAltitudeDetected(float threshold = 300.0, float min_altitude = 100.0, int times = 25) //threshold - threshold altitude to be detected (in m), min_altitude - minimum altitude at which the main ejection conditions can be detected (part of low main ejection safety) times - times to detect altitude under threshold altitude for main ejection
    {
        static int counter = 0; 
        if (f_alt < threshold && f_alt > min_altitude) // Note: if altitude is below minimum ejection altitude the flash will still be written to, and will need clearing after
        {
            counter++;
        }
        else if (f_vel < abn_speed) // if abnormal descent speed is detected
        {
            counter++;
        }
        else
        {
            counter = 0;
        }
        if (counter > times)
        {
            Serial.println("Main ejection altitude detected!");
            return true;
        }
        else
        {
            return false;
        }
    }

    sens_data::BarometerData getBarometerState() //*outputs sensor readings from wrapper to be used in the data object
    {
        sens_data::BarometerData bd;
        bd.temperature = temp;
        bd.pressure = pres / 100.0; //saving in hPa
        bd.altitude = alt;
        bd.f_altitude = f_alt;
        bd.f_velocity = f_vel;
        bd.f_acceleration = f_acc;
        return bd;
    }
}