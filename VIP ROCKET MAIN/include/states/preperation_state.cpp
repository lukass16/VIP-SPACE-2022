#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "drogue_state.cpp"

class PreperationState: public State {
    public: 

        void start () override {
            Serial.println("PREP STATE");

            delay(2000);

            this->_context->RequestNextPhase();
            this->_context->Start();
        }

        void HandleNextPhase() override {
            this->_context->TransitionTo(new DrogueState);
        }
};