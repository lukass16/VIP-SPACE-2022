#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "communication.h"
#include "eeprom_wrapper.h"
#include "arming.h"
#include "flash.h"
#include "SD_card.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_BNO055.h"

class DescentState : public State
{
public:
    void start() override
    {
        Serial.println("DESCENT STATE");

        SD_File fileSD = SDcard::openFile();

        File file = flash::openFile(); // opening flash file for writing during descent
        int flash_counter = 0, flash_write_time = 140000;
        int interval = 200;           // amount of loops after which the flash is closed and opened
        int descent_state_delay = 15; // delay used in descent state [ms]
        //? A descent state delay time of 16 ms means an approximate frequency of 50 Hz, amount of data written in this state (50 s) is about 5000 loop buffers
        s_data.setRocketState(3); // set rocket state to descent (4) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        // start touchdown detection timer
        arming::startTouchdownTimer();

        while (!arming::timerDetectTouchdown()) // while touchdown has not been detected by timer
        {
            buzzer::signalDescent();

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
            if (!flash::flashEnded(file, flash_write_time)) // if not finished writing to flash
            {
                flash_counter = flash::writeData(file, gd, md, bd, btd, 4); // writing data to flash memory
                if (flash_counter % interval == 1)
                {
                    file = flash::closeOpen(file); // close and open the file every 100th reading
                }
            }

            //* writing to SD card
            SDcard::writeDataStruct(fileSD, flash::getTimeElapsed(), gd, md, bd, btd, 3);

            delay(descent_state_delay);
        }

        //*close SD file
        SDcard::closeFile(fileSD);

        while (true) // post touchdown operations
        {
            buzzer::signalTouchdown();

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

            delay(descent_state_delay);
        }
    }

    void HandleNextPhase() override
    {
        Serial.println("END of RTU HPR ROCKET CODE");
    }
};
