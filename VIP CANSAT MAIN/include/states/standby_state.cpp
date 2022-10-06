#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "main_state.cpp"
#include "communication.h"
#include "flash.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_BNO055.h"
#include "eeprom_wrapper.h"
#include "arming.h"
#include "SD_card.h"

//*Temporary variables
bool clearEEPROM = true;

class StandByState : public State
{
public:
    void start() override
    {

        Serial.println("STANDBY STATE");

        int standby_state_delay = 50; // delay used in preparation state [ms]

        s_data.setRocketState(1); // set rocket state to preparation (1) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        //*Buzzer setup - signal start
        buzzer::setup();
        buzzer::transitionToGeneratorMode();

        //*EEPROM setup
        eeprom::setup();

        //*Arming setup
        arming::setup();

        //*Flash setup
        flash::setup();
        
        //*SD card setup
        SDcard::setup();
        SDcard::getFileName();
        SD_File fileSD = SDcard::openFile();
        SDcard::writeHeader(fileSD);

        //*Sensor setups
        Wire.begin(21, 22); // initialize correct i2c lines
        gps::setup();
        barometer::setup();
        imu::setup();


        // Signal battery voltage
        buzzer::signalBatteryVoltage();
        

        //*check if need to clear EEPROM
        if (arming::clearEEPROM())
        {
            buzzer::signalEEPROMClear();
            eeprom::clean();
        }

        //*if flash not locked - delete file
        if (!eeprom::lockedFlash())
        {
            flash::deleteFile("/data.txt"); //*deleting file so as to reset it
        }


        //*perform barometer ground pressure sampling and save sampled pressure value to EEPROM
        barometer::sampleSeaLevel();


        // Detect launch using IMU acceleration *a* for *n* times, or if has been launch - skip
        while (!imu::launchDetected())
        {
            buzzer::signalStandBy();

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

            //give necessary feedback during loop
            //imu::printAll();

            //* writing to SD card
            SDcard::writeDataStruct(fileSD, flash::getTimeElapsed(), gd, md, bd, btd, 1);

            delay(standby_state_delay);
        }

        //*close SD file
        SDcard::closeFile(fileSD);

        eeprom::lockFlash();

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new MainState);
    }

};