#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "main_state.cpp"

class DrogueState: public State {
    public: 

        void start () override {
            Serial.println("DROGUE STATE");

            delay(2000);

            this->_context->RequestNextPhase();
            this->_context->Start();
        }

        void HandleNextPhase() override {
            this->_context->TransitionTo(new MainState);
        }
};