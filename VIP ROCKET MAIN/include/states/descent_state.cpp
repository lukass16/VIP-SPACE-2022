#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "flash.h"
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

        File file = flash::openFile(); // opening flash file for writing during descent
        int flash_counter = 0, flash_write_time = 30000;

        s_data.updateRocketState(); // update state that's written to LoRa messages

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        while (true)
        {
            buzzer::signalDescent();

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

            if (!flash::flashEnded(file, flash_write_time))
            {
                flash_counter = flash::writeData(file, gd, md, bd, btd, 4); // writing data to flash memory
                if (flash_counter % 100 == 1)
                {
                    file = flash::closeOpen(file); // close and open the file every 100th reading
                }
            }
            delay(200);
        }
    }

    void HandleNextPhase() override
    {
        Serial.println("END of VIP ROCKET CODE");
    }
};
