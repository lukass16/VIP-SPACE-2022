#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "communication.h"
#include "arming.h"
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

        File file = flash::openFile(); // opening flash file for writing during descent
        int flash_counter = 0, flash_write_time = 10000;

        SD_File fileSD;

        s_data.setRocketState(4); // set rocket state to descent (4) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        //start touchdown detection timer
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

            //give necessary feedback during loop
            barometer::printState();

            if (!flash::flashEnded(file, flash_write_time)) //if not finished writing to flash
            {
                flash_counter = flash::writeData(file, gd, md, bd, btd, 4); // writing data to flash memory
                if (flash_counter % 100 == 1)
                {
                    file = flash::closeOpen(file); // close and open the file every 100th reading
                }
            }

            delay(50);
        }


        unsigned long process_start_time = millis();

        //* stop communication to not interfere with writing to SD card
        comms::stop();

        SDcard::setup();
        fileSD = SDcard::openNextFile();
        flash::dumpContentsToSD("/data.txt", fileSD);
        SDcard::closeFile(fileSD);

        //* resume communication
        comms::resume();

        Serial.println("Process took: " + String(millis() - process_start_time) + " ms");


        while (true) // post touchdown operations
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

            //give necessary feedback during loop
            barometer::printState();

            delay(200);
        }
    }

    void HandleNextPhase() override
    {
        Serial.println("END of RTU HPR ROCKET CODE");
    }
};
