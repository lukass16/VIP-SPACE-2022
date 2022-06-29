#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "flash.h"
#include "SD_card.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_MPU9250.h"

class DescentState : public State
{
public:
    void start() override
    {
        Serial.println("DESCENT STATE");

        File file = flash::openFile();       // opening flash file for writing during descent
        SD_File fileSD = SDcard::openFile(); // opening SD file for writing
        SDcard::markDescent(fileSD);

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        while(!flash::flashEnded(file))
        {
             //*gps
            gps::readGps();          // reads in values from gps
            gd = gps::getGpsState(); // retrieve values from wrapper to be put in data object
            s_data.setGpsData(gd);

            //*barometer
            barometer::readSensor();
            barometer::printState();
            bd = barometer::getBarometerState(); // reads and retrieves values from wrapper to be put in data object
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            md = imu::getIMUState();
            s_data.setIMUData(md);

            //*placeholder for battery data

            
            if(!flash::flashEnded(file))
            {
                // writing to flash
                flash::writeData(file, gd, md, bd, btd);
            }
            
            // writing to SD card
            SDcard::writeData(fileSD, gd, md, bd, btd);
            delay(200);
        }

        flash::readFlashVerbose("/test.txt");

        delay(200000000);

    }

    void HandleNextPhase() override
    {
        Serial.println("END of VIP ROCKET CODE");
    }
};
