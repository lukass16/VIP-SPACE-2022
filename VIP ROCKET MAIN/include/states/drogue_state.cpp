#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "main_state.cpp"
#include "flash.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper.h"
#include "imu_wrapper.h"

class DrogueState : public State
{
public:
    void start() override
    {
        Serial.println("DROGUE STATE");

        //*Flash testing
        Serial.println("Testing flash");
        flash::deleteFile("/test.txt"); //*deleting file so as to reset it every test
        File file = flash::openFile();  // opening file for writing during flight

        //*variables for writing to flash (Note: in the preparation state these are repeatedly constructed every loop - should evaluate whether defining them once would not be a better solution)
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        while (flash::writeData(file, gd, md, bd, btd) <= 100) //*while a 100 writes have not been made
        {
            //*gps
            gps::readGps();          // reads in values from gps
            gd = gps::getGpsState(); // retrieve values from wrapper to be put in data object
            s_data.setGpsData(gd);

            //*barometer
            bd = barometer::getBarometerState(); // reads and retrieves values from wrapper to be put in data object
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            md = imu::getIMUState();
            s_data.setIMUData(md);

            //*placeholder for battery data

            delay(100);
        }

        flash::closeFile(file);
        Serial.println("Finished writing to flash");

        flash::readFlashVerbose("/test.txt"); // opening and reading file
        while (1);

        delay(2000);

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new MainState);
    }
};