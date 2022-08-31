#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "main_state.cpp"
#include "flash.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_MPU9250.h"
#include "eeprom_wrapper.h"
#include "arming.h"

class DrogueState : public State
{
public:
    void start() override
    {
        Serial.println("DROGUE STATE");

        File file = flash::openFile(); // opening flash file for writing during flight
        int flash_counter = 0;
        int interval = 100; // amount of loops after which the flash is closed and opened

        s_data.setRocketState(2); // set rocket state to drogue (2) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        // Detect launch using IMU acceleration *a* for *n* times, or if has been launch - skip
        while (!imu::launchDetected() && !eeprom::hasBeenLaunch())
        {
            buzzer::signalDrogue();

            //*gps
            gps::readGps();
            gd = gps::getGpsState();
            s_data.setGpsData(gd);

            //*barometer
            barometer::readSensor();
            bd = barometer::getBarometerState();
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            md = imu::getIMUState();
            s_data.setIMUData(md);

            //*battery data

            //give necessary feedback during loop
            imu::printAll();

            delay(50);
        }

        //mark launch in EEPROM
        eeprom::markLaunch();
        eeprom::lockFlash();

        //start apogee detection timer
        arming::startApogeeTimer();

        while (!arming::timerDetectApogee())
        {
            buzzer::signalDrogue();
            
            //*gps
            gps::readGps();
            gd = gps::getGpsState();
            s_data.setGpsData(gd);

            //*barometer
            barometer::readSensor();
            bd = barometer::getBarometerState();
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            md = imu::getIMUState();
            s_data.setIMUData(md);

            //*battery data

            //give necessary feedback during loop
            barometer::printState();

            flash_counter = flash::writeData(file, gd, md, bd, btd, 2); // writing data to flash memory
            if (flash_counter % interval == 1)
            {
                file = flash::closeOpen(file); // close and open the file every 100th reading
            }
            
            delay(50);
        }

        // mark apogee in EEPROM
        eeprom::markApogee();

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