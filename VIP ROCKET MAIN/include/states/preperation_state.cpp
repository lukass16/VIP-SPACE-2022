#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "drogue_state.cpp"
#include "oled_wrapper.h"
#include "communication.h"
#include "wifiserver_wrapper.h"
#include "flash.h"
#include "buzzer.h"
#include "gps_wrapper.h"
#include "barometer_wrapper_MS5607.h"
#include "imu_wrapper_MPU9250.h"
#include "eeprom_wrapper.h"
#include "SD_card.h"

//*Temporary variables
bool clearEEPROM = true;

class PreperationState : public State
{
public:

    void start() override
    {

        Serial.println("PREP STATE");

        //*Buzzer test/signal start
        buzzer::setup();
        buzzer::signalStart();
        delay(500);
        buzzer::buzzEnd();

        //*EEPROM setup
        eeprom::setup();

        //*flash setup
        flash::setup();

        //*SD setup
        SDcard::setup();

        //*Sensor setups
        Wire.begin(21, 22); // initialize correct i2c lines
        gps::setup();
        barometer::setup();
        imu::setup();

        comms::setup(433E6);
        s_data.updateRocketState(); //update state that's written to LoRa messages

        //*check if need to clear EEPROM
        if(clearEEPROM) //TODO add dedicated clear function which clears all appropriate EEPROM addresses
        {
            eeprom::unlockFlash(); //only utility currently for EEPROM
        }

        //*if flash not locked - delete file
        if(!eeprom::lockedFlash())
        {
            flash::deleteFile("/test.txt"); //*deleting file so as to reset it
        }

        //TODO add EEPROM state transfer mechanism

        int loops = 0; 
        //!TODO change with while(!arming::armed) - add arming functionality 
        while (true)
        {
            //*gps
            gps::readGps();                             // reads in values from gps
            sens_data::GpsData gd = gps::getGpsState(); // retrieve values from wrapper to be put in data object
            s_data.setGpsData(gd);

            //*barometer
            barometer::readSensor();
            sens_data::BarometerData bd = barometer::getBarometerState(); //reads and retrieves values from wrapper to be put in data object
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            sens_data::IMUData md = imu::getIMUState();
            s_data.setIMUData(md);

            //*battery
            sens_data::BatteryData btd;

            delay(50);
            // loops++;
            // Serial.println(loops);
        }

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new DrogueState);
    }
};