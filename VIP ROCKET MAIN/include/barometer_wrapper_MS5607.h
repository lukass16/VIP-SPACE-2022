#pragma once
#include <Wire.h>
#include <MS5x.h>
#include "sensor_data.h"
#include "kalman.h"

namespace barometer
{
    MS5x MS5607(&Wire);

    double temp = 0;
    double pres = 0;
    double alt = 0;
    double seaLevelPressure = 0;

    //Kalman (filtered) readings
    float f_alt = 0;
    float f_vel = 0;
    float f_acc = 0;

    unsigned long kalman_t; // Kalman timer
    float t_change = 0;     // change of time from previous prediction to current

    //*for testing
    unsigned long start_t = 0; //*for testing
    int counter = 0;

    void setup()
    {
        start_t = millis();

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
            counter++;
            MS5607.checkUpdates();
            if (MS5607.isReady())
            {
                // Calculate predicted seaLevel pressure based off a known altitude in meters
                seaLevelPressure = MS5607.getSeaLevel(10.5); // this functions also as the sea level setter for altitude calculations //!This can only be used to setup the barometer on the ground (ground level)
                Serial.println("Sea level pressure set as: " + String(seaLevelPressure));
                Serial.println("Time ellapsed while barometer ready: " + String(millis() - start_t, 5)); //*the delay is about 44 ms which is quite okay - it results from multiple loops while the sensor gets first readings
                Serial.println("Cycles ellapsed while barometer ready: " + String(counter));
            }
        }

        //kalman setup
        kalman::predict(); // make first prediction
	    kalman_t = millis();

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

        // kalman implementation
        t_change = (millis() - kalman_t) / 1000.0;
        kalman::setDelT(t_change);

        kalman_t = millis();

        kalman::predict();
	    kalman::updateBaro(alt);

        //set filtered readings
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

    float getVertVelocity() // todo make this
    {
        return 42.0;
    }

    sens_data::BarometerData getBarometerState() //*outputs sensor readings from wrapper to be used in the data object
    {
        sens_data::BarometerData bd;
        bd.temperature = temp;
        bd.pressure = pres;
        bd.altitude = alt;
        bd.f_altitude = f_alt;
        bd.f_velocity = f_vel;
        bd.f_acceleration = f_acc;
        return bd;
    }
}