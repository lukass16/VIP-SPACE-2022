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

class PreperationState : public State
{
public:
    float it = 0;

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

        //*SD card setup
        SDcard::setup(); 
        SD_File fileSD = SDcard::openFile();

        //*Sensor setups
        Wire.begin(21, 22); // initialize correct i2c lines
        gps::setup();
        barometer::setup();
        imu::setup();

        comms::setup(433E6);

        //*testing wifi in prep loop
        /*  wifiserver::setup(); 
        while(true)
        {
            it++;
            sens_data::MagenetometerData md;
            md.x = it;
            s_data.setMagnetometerData(md); //setting data in sensor data object
            wifiserver::setData(); //making the wifi server retrieve/update the data
            wifiserver::handleClient();
            delay(100);
        }*/

        delay(2000);

        //*Sensor reading test loop + SD_Card read, write test loop
        int loops = 0;
        while (loops<50) //!change for flash testing
        {
            //*gps
            gps::readGps();                             // reads in values from gps
            sens_data::GpsData gd = gps::getGpsState(); // retrieve values from wrapper to be put in data object
            s_data.setGpsData(gd);

            //*barometer
            barometer::readSensor();
            sens_data::BarometerData bd = barometer::getBarometerState(); //reads and retrieves values from wrapper to be put in data object
            s_data.setBarometerData(bd);
            barometer::printInfoFiltered(); //*Testing Kalman

            //*imu
            imu::readSensor();
            sens_data::IMUData md = imu::getIMUState();
            s_data.setIMUData(md);

            //*battery
            sens_data::BatteryData btd;

            //*testing SD card
            SDcard::writeData(fileSD, gd, md, bd, btd);

            delay(50);
            loops++;
        }

        //*close files
        SDcard::closeFile(fileSD);

        //!Stop for kalman testing
        while(true);
        
        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new DrogueState);
    }
};