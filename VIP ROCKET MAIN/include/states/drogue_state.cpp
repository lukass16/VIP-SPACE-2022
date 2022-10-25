#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "main_state.cpp"
#include "flash.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_BNO055.h"
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
        int drogue_state_delay = 4; // delay used in drogue state [ms]

        s_data.setRocketState(2); // set rocket state to launch wait (2) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        // Detect launch using IMU acceleration *a* for *n* times, or if has been launch - skip
        while (!imu::launchDetected() && !eeprom::hasBeenLaunch())
        {
            buzzer::signalDrogue1();

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
            arming::readBatteryVoltage();
            btd = arming::getBatteryState();
            s_data.setBatteryData(btd);

            delay(drogue_state_delay);
        }

        //mark launch in EEPROM
        eeprom::markLaunch();
        eeprom::lockFlash();

        s_data.setRocketState(3); // set rocket state to apogee wait (3) state

        //start apogee detection timer
        arming::startApogeeTimer();

        while (!arming::timerDetectApogee())
        {
            buzzer::signalDrogue2();
            
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
            arming::readBatteryVoltage();
            btd = arming::getBatteryState();
            s_data.setBatteryData(btd);

            flash_counter = flash::writeData(file, gd, md, bd, btd, 3); // writing data to flash memory
            if (flash_counter % interval == 1)
            {
                file = flash::closeOpen(file); // close and open the file every 100th reading
            }
            
            delay(drogue_state_delay);
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