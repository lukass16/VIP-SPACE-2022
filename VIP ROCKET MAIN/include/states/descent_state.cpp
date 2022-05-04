#pragma once

#include "Arduino.h"
#include "core/core.cpp"

class DescentState : public State {
    public:
        void start() override
        {
            Serial.println("DESCENT STATE");

            Serial.println("END");
            while(1);
        }

        void HandleNextPhase() override
        {
            Serial.println("END of VIP ROCKET CODE");
        }
};