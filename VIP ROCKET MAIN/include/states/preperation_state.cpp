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
#include "barometer_wrapper_bmp280.h"
#include "imu_wrapper_MPU9250.h"
#include "EEPROM.h"

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

        // oled::setup(); //*possibly conflicting with GPS

        //*flash setup
        flash::setup();

        //*Sensor setups
        Wire.begin(21, 22); // initialize correct i2c lines
        gps::setup();
        barometer::setup();
        imu::setup();

        comms::setup(433E6);

        /*  wifiserver::setup(); //*testing wifi in prep loop
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

        //*Sensor reading test loop
        while (true) //!change for flash testing
        {
            //*gps
            gps::readGps();                             // reads in values from gps
            sens_data::GpsData gd = gps::getGpsState(); // retrieve values from wrapper to be put in data object
            s_data.setGpsData(gd);

            //*barometer
            sens_data::BarometerData bd = barometer::getBarometerState(); //reads and retrieves values from wrapper to be put in data object
            s_data.setBarometerData(bd);

            //*imu
            imu::readSensor();
            sens_data::IMUData md = imu::getIMUState();
            s_data.setIMUData(md);

            delay(1000);
        }

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new DrogueState);
    }
};