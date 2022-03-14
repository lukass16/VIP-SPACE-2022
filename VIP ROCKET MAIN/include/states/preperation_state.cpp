#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "drogue_state.cpp"
#include "oled_wrapper.h"
#include "communication.h"
#include "wifiserver_wrapper.h"
#include "EEPROM.h"

class PreperationState: public State {
    public: 

        float it = 0;

        void start () override {
            
            Serial.println("PREP STATE");
            oled::setup();

            comms::setup(433E6);
            wifiserver::setup();

            while(true) //*testing wifi in prep loop 
            {
                it++;
                sens_data::MagenetometerData md;
                md.x = it;
                s_data.setMagnetometerData(md); //setting data in sensor data object
                wifiserver::setData(); //making the wifi server retrieve/update the data
                wifiserver::handleClient();
                delay(100);
            }

            delay(2000);

            this->_context->RequestNextPhase();
            this->_context->Start();
        }

        void HandleNextPhase() override {
            this->_context->TransitionTo(new DrogueState);
        }
};