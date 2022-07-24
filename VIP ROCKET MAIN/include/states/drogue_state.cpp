#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "main_state.cpp"
#include "flash.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_MPU9250.h"

class DrogueState : public State
{
public:
    void start() override
    {
        Serial.println("DROGUE STATE");

        File file = flash::openFile();       // opening flash file for writing during flight
        s_data.updateRocketState(); //update state that's written to LoRa messages

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        // Detect launch using IMU acceleration *a* for *n* times
        while (!imu::launchDetected())
        {
            //*gps
            gps::readGps();          // reads in values from gps
            gd = gps::getGpsState(); // retrieve values from wrapper to be put in data object
            s_data.setGpsData(gd);

            //*barometer
            barometer::readSensor();
            bd = barometer::getBarometerState(); // reads and retrieves values from wrapper to be put in data object
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            imu::printAll();
            md = imu::getIMUState();
            s_data.setIMUData(md);

            //*placeholder for battery data

            // writing to flash
            flash::writeData(file, gd, md, bd, btd, 2);
            delay(50);
        }

        // close flash file
        flash::closeFile(file);
        flash::readFlashVerbose("/test.txt");
        while(true); //!Flash testing

        while (!barometer::apogeeDetected()) //TODO add alternative timer apogee detection
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

            // writing to flash
            flash::writeData(file, gd, md, bd, btd, 2);
            delay(50);
        }

        // close flash file
        flash::closeFile(file);

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new MainState);
    }
};