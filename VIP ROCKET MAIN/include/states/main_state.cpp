#pragma once

#include "Arduino.h"
#include "core/core.cpp"
#include "descent_state.cpp"

class MainState : public State {
    private:
        bool timerEnabled = 0;  
    public:
        void start() override
        {
            Serial.println("MAIN STATE");

            delay(1000);
            while(true)
            {
                delay(1000);
            }
           
            this->_context->RequestNextPhase();
            this->_context->Start();
        }

        void HandleNextPhase() override
        {
            this->_context->TransitionTo(new DescentState());
        }
};
