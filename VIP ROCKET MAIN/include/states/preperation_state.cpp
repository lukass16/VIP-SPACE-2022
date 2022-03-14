#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "drogue_state.cpp"
#include "oled_wrapper.h"
#include "communication.h"
#include "EEPROM.h"

class PreperationState: public State {
    public: 

        void start () override {
            
            Serial.println("PREP STATE");
            oled::setup();

            delay(2000);

            this->_context->RequestNextPhase();
            this->_context->Start();
        }

        void HandleNextPhase() override {
            this->_context->TransitionTo(new DrogueState);
        }
};