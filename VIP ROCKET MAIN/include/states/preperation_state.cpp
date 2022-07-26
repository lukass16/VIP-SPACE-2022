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
#include "arming.h"
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
        buzzer::transitionToGeneratorMode();
        buzzer::signalStart();

        //*EEPROM setup
        eeprom::setup();

        //*Arming setup
        arming::setup();

        //*Flash setup
        flash::setup();

        // //*SD setup
        // SDcard::setup();

        //*Sensor setups
        Wire.begin(21, 22); // initialize correct i2c lines
        gps::setup();
        barometer::setup();
        imu::setup();

        comms::setup(433E6);
        s_data.updateRocketState(); //update state that's written to LoRa messages

        //*check if need to clear EEPROM
        if(arming::clearEEPROM())
        {
            buzzer::signalEEPROMClear();
            eeprom::clean(); //only utility currently for EEPROM
        }

        //*if flash not locked - delete file
        if(!eeprom::lockedFlash())
        {
            flash::deleteFile("/test.txt"); //*deleting file so as to reset it
        }

        //*Determine if has been reset - need to transfer to different state
        eeprom::readPreviousState();
        if(eeprom::hasBeenLaunch())
        {
            JumpToDrogue();
            this->_context->Start();
        }
        else if(eeprom::hasBeenApogee())
        {
            JumpToMain();
            this->_context->Start();
        }
        else if(eeprom::hasBeenMainEjection())
        {
            JumpToDescent();
            this->_context->Start();
        }

        int loops = 0; 
         
        while (loops < 100) //!TODO change with while(!arming::armed) - add arming functionality
        {
            buzzer::signalNotArmed();

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
            loops++;
            Serial.println(loops);
        }

        buzzer::signalArmed();

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new DrogueState);
    }

    //Jumping functions for EEPROM transfer mechanism
    void JumpToDrogue()
    {
        this->_context->TransitionTo(new DrogueState);
    }

    void JumpToMain()
    {
        this->_context->TransitionTo(new MainState);
    }

    void JumpToDescent()
    {
        this->_context->TransitionTo(new DescentState);
    }
};