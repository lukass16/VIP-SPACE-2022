#pragma once

#include "Arduino.h"
#define RESONATING_FREQ 2500

namespace buzzer
{
    //declaring a variable for the piezo pin
    int piezo_pin = 23;

    //variable for the structure of the third switch beeping
    bool thirdSwitchBeep = false, flightBeep = false, descentBeep = false;
    unsigned long previousTime = 0, currentTime = 0;

    //declaring variables for PWM channel attributes
    int freq = 400, channel = 0, resolution = 8;

    void setup()
    {
        ledcSetup(channel, freq, resolution); //setting up the PWM channel
        ledcAttachPin(piezo_pin, channel);    //attaching the piezo_pin to the PWM channel

        Serial.println("Buzzer ready!");
    }

    void transitionToGeneratorMode()
    {
        ledcDetachPin(piezo_pin);
        pinMode(piezo_pin, OUTPUT);
    }

    void setPiezoPin(u_int piezo_pin)
    {
        buzzer::piezo_pin = piezo_pin;
    }

    void buzz(uint freq = freq)
    {
        ledcWriteTone(channel, freq);
        ledcWrite(channel, 127);
    }

    void buzzDigital()
    {
        digitalWrite(piezo_pin, HIGH);
    }

    void buzzCustom(u_int soundLevel, uint freq = freq)
    {
        ledcWriteTone(channel, freq);
        ledcWrite(channel, soundLevel);
    }

    void buzzEnd()
    {
        buzzer::buzzCustom(0, 0);
    }

    void buzzEndDigital()
    {
        digitalWrite(piezo_pin, LOW);
    }

    void signalTestStart()
    {
        buzzer::buzz(900);
        delay(300);
        buzzer::buzz(1080);
        delay(300);
        buzzer::buzzEnd();
    }

    void signalTransition()
    {
        digitalWrite(piezo_pin, HIGH);
        delay(70);
        digitalWrite(piezo_pin, LOW);
        delay(70);
        digitalWrite(piezo_pin, HIGH);
        delay(70);
        digitalWrite(piezo_pin, LOW);
    }

    void signalCalibrationEnd()
    {
        buzzer::buzz(1080);
        delay(300);
        buzzer::buzz(900);
        delay(300);
        buzzer::buzzEnd();
    }

    void signalSecondSwitch()
    {
        buzzer::buzz(1080);
        delay(100);
        buzzer::buzzEnd();
        delay(50);
        buzzer::buzz(1080);
        delay(100);
        buzzer::buzzEnd();
    }

    void signalThirdSwitchLockout()
    {
        int interval = 1000; //interval time in milliseconds
        currentTime = millis();
        if (currentTime - previousTime >= interval)
        {
            previousTime = currentTime; //save the last time that buzzer was toggled
            if (!thirdSwitchBeep)       //if not buzzing
            {
                buzzer::buzz(520);
                thirdSwitchBeep = true;
            }
            else
            {
                buzzer::buzzEnd();
                thirdSwitchBeep = false;
            }
        }
    }

    void signalFlight()
    {
        int interval = 500; //interval time in milliseconds
        currentTime = millis();
        if (currentTime - previousTime >= interval)
        {
            previousTime = currentTime; //save the last time that buzzer was toggled
            if (!flightBeep)       //if not buzzing
            {
                digitalWrite(piezo_pin, HIGH);
                flightBeep = true;
            }
            else
            {
                digitalWrite(piezo_pin, LOW);
                flightBeep = false;
            }
        }
    }

    void signalDescent() //identical to previous function
    {
        int interval = 2000; //interval time in milliseconds
        currentTime = millis();
        if (currentTime - previousTime >= interval)
        {
            previousTime = currentTime; //save the last time that buzzer was toggled
            if (!descentBeep)       //if not buzzing
            {
                digitalWrite(piezo_pin, HIGH);
                descentBeep = true;
            }
            else
            {
                digitalWrite(piezo_pin, LOW);
                descentBeep = false;
            }
        }
    }

    void signalSavedValues()
    {
        buzzer::buzz(480);
        delay(500);
        buzzer::buzz(860);
        delay(500);
        buzzer::buzz(1080);
        delay(500);
        buzzer::buzzEnd();
    }

    void signalEEPROMClear()
    {
        buzzer::buzz(2000);
        delay(200);
        buzzer::buzzEnd();
        delay(200);
        buzzer::buzz(2000);
        delay(200);
        buzzer::buzzEnd();        
        delay(2000); //delay to differentiate signals
    }

    void test()
    {
        buzzer::buzzCustom(127, 200);
        delay(500);
        buzzer::buzzCustom(127, 500);
        delay(500);
        buzzer::buzzCustom(127, 800);
        delay(500);
        buzzer::buzzEnd();
    }

}