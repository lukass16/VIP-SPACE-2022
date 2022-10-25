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

        File file = flash::openFile(); // opening flash file for writing during descent
        int flash_counter = 0, flash_write_time = 60000;
        int interval = 100; // amount of loops after which the flash is closed and opened
        int descent_state_delay = 46; // delay used in descent state [ms]

        SD_File fileSD;

        s_data.setRocketState(5); // set rocket state to descent (5) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        //*start touchdown detection timer
        arming::startTouchdownTimer();

        while (!arming::timerDetectTouchdown() && !eeprom::hasBeenTouchdown()) // while touchdown has not been detected by timer or has not already been detected
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

            if (!flash::flashEnded(file, flash_write_time)) //if not finished writing to flash
            {
                flash_counter = flash::writeData(file, gd, md, bd, btd, 5); // writing data to flash memory
                if (flash_counter % interval == 1)
                {
                    file = flash::closeOpen(file); // close and open the file every 100th reading
                }
            }

            delay(descent_state_delay);
        }

        //*mark touchdown in EEPROM
        eeprom::markTouchdown();

        s_data.setRocketState(6); // set rocket state to touchdown (6) state

        //*Dump flash contents to SD card
        comms::stop(); // stop communication to not interfere with writing to SD card
        SDcard::setup(); // setup SD card
        fileSD = SDcard::openNextFile();
        flash::dumpContentsToSD("/data.txt", fileSD);
        SDcard::closeFile(fileSD);
        comms::resume(); // resume communication


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

            delay(descent_state_delay);
        }
    }

    void HandleNextPhase() override
    {
        Serial.println("END of RTU HPR ROCKET CODE");
    }
};
