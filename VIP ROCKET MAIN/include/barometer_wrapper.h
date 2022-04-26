#pragma once
#include <Adafruit_BMP280.h>
#include "sensor_data.h"

#define AVIATION_PRESSURE 1013.5

//! NOTE: There is currently an error when suing this Adafruit library - this can be fixed by removing the const attributes from the used variables of two functions

namespace barometer
{
    Adafruit_BMP280 bmp; // I2C Interface

    float sea_level_sample = 0;
    // defining necessary variables for vertical velocity calculation
    float h_now = 0, h_prev = 0, t_now = 0, t_prev = 0, dh = 0, dt = 0, vert_velocity = 0, vert_velocity_prev = 0;

    float getVertVelocity() //todo make this
    {
        return 42.0;
    }

    void setup(bool read_sea_level = 0)
    {
        if (!bmp.begin(0x76))
        {
            Serial.println("Could not find a valid BMP280 sensor, check wiring!");
            while (1);
        }

        /* Default settings from datasheet. */
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
        //*if needed start sampling sea level pressure
        if (read_sea_level)
        {
            for (int i = 0; i < 5; i++)
            {
                sea_level_sample += (bmp.readPressure() / 100);
            }
            sea_level_sample = sea_level_sample / 5;
        }
        else
        {
            sea_level_sample = AVIATION_PRESSURE;
        }
        Serial.println("Barometer ready! Sea level pressure set as: " + String(sea_level_sample));
    }

    void printSensorValues()
    {
        Serial.print("Temperature = ");
        Serial.print(bmp.readTemperature());
        Serial.println(" *C");

        Serial.print("Pressure = ");
        Serial.print(bmp.readPressure() / 100); // displaying the Pressure in hPa, you can change the unit
        Serial.println(" hPa");

        Serial.print("Approx altitude = ");
        Serial.print(bmp.readAltitude(sea_level_sample));
        Serial.println(" m");
        Serial.println();
    }

    sens_data::BarometerData getBarometerState() //*outputs sensor readings from wrapper to be used in the data object
    {
        sens_data::BarometerData bd;
        bd.temperature = bmp.readTemperature();
        bd.pressure = bmp.readPressure() / 100;
        bd.altitude = bmp.readAltitude(sea_level_sample);
        bd.vert_velocity = getVertVelocity();
        return bd;
    }

}