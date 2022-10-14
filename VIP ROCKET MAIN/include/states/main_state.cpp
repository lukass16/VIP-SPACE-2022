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

        File file = flash::openFile();       // opening flash file for writing during flight
        int flash_counter = 0;
        int interval = 100; // amount of loops after which the flash is closed and opened
        int main_state_delay = 16; // delay used in main state [ms]

        s_data.setRocketState(4); // set rocket state to main wait (4) state

        // variables for writing to memory
        sens_data::GpsData gd;
        sens_data::BarometerData bd;
        sens_data::IMUData md;
        sens_data::BatteryData btd;

        while (!barometer::mainAltitudeDetected()) //*waiting until altitude is below threshold to eject main parachute
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

            //give necessary feedback during loop
            //barometer::printState();

            flash_counter = flash::writeData(file, gd, md, bd, btd, 4); // writing data to flash memory
            if (flash_counter % interval == 1)
            {
                file = flash::closeOpen(file); // close and open the file every 100th reading
            }

            delay(main_state_delay);
        }

        buzzer::signalWarningMainEjection();

        //fire main parachute pyro charge
        arming::fireMainCharge(); //! commented out for safety

        //mark main ejection in EEPROM
        eeprom::markMainEjection();

        // close flash file
        flash::closeFile(file);

        this->_context->RequestNextPhase();
        this->_context->Start();
    }

    void HandleNextPhase() override
    {
        this->_context->TransitionTo(new DescentState());
    }
};
