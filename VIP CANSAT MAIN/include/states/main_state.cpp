#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "descent_state.cpp"
#include "flash.h"
#include "arming.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_BNO055.h"
#include "eeprom_wrapper.h"
#include "arming.h"

class MainState : public State
{
public:
    void start() override
    {
        Serial.println("MAIN STATE");

        SD_File fileSD = SDcard::openFile();

        File file = flash::openFile(); // opening flash file for writing during flight
        int flash_counter = 0;
        int interval = 200;        // amount of loops after which the flash is closed and opened
        int main_state_delay = 0; // delay used in main state [ms]
        //? A main state delay time of 4 ms means an approximate frequency of 125 Hz, amount of data written in this state (20 s) is about 2500 loop buffers
        s_data.setRocketState(2); // set rocket state to main (3) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        // start apogee detection timer
        arming::startApogeeTimer();

        while (!arming::timerDetectApogee())
        {
            buzzer::signalMain();

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

            // give necessary feedback during loop
            // barometer::printState();

            //*writing to flash
            flash_counter = flash::writeData(file, gd, md, bd, btd, 2); // writing data to flash memory
            if (flash_counter % interval == 1)
            {
                file = flash::closeOpen(file); // close and open the file every 100th reading
            }

            //* writing to SD card
            SDcard::writeDataStruct(fileSD, flash::getTimeElapsed(), gd, md, bd, btd, 2);

            delay(main_state_delay);
        }

        //*close SD file
        SDcard::closeFile(fileSD);

        //*close flash file
        flash::closeFile(file);

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new DescentState());
    }
};
