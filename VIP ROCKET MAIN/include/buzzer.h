#pragma once

#include "Arduino.h"
#include "arming.h"

#define RESONATING_FREQ 2500
#define BOP 200
#define BEEP 1000

namespace buzzer
{
    // declaring a variable for the piezo pin
    int piezo_pin = 23;

    // variable for the structure of the third switch beeping
    bool buzzing = false;
    unsigned long previousTime = 0, currentTime = 0;
    int times_buzzed = 0;

    // declaring variables for PWM channel attributes
    int freq = 400, channel = 0, resolution = 8;

    void setup()
    {
        ledcSetup(channel, freq, resolution); // setting up the PWM channel
        ledcAttachPin(piezo_pin, channel);    // attaching the piezo_pin to the PWM channel

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

    void buzzCustom(u_int soundLevel, uint freq = freq)
    {
        ledcWriteTone(channel, freq);
        ledcWrite(channel, soundLevel);
    }

    void buzzEnd()
    {
        buzzer::buzzCustom(0, 0);
    }

    void signalStart()
    {
        digitalWrite(piezo_pin, HIGH);
        delay(300);
        digitalWrite(piezo_pin, LOW);
    }

    void signalCalibrationStart()
    {
        buzzer::buzz(900);
        delay(300);
        buzzer::buzz(1080);
        delay(300);
        buzzer::buzzEnd();
    }

    void signalCalibrationEnd()
    {
        buzzer::buzz(1080);
        delay(300);
        buzzer::buzz(900);
        delay(300);
        buzzer::buzzEnd();
    }

    void signalCalibrationSkip()
    {
        Serial.println("Calibration skipped - EEPROM shows as calibrated");
        signalCalibrationStart();
        signalCalibrationEnd();
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

    void signalEEPROMClearLegacy()
    {
        buzzer::buzz(2000);
        delay(200);
        buzzer::buzzEnd();
        delay(200);
        buzzer::buzz(2000);
        delay(200);
        buzzer::buzzEnd();
        delay(2000); // delay to differentiate signals
    }

    void signalEEPROMClear()
    {
        digitalWrite(piezo_pin, HIGH); // 1)
        delay(50);
        digitalWrite(piezo_pin, LOW);
        delay(50);
        digitalWrite(piezo_pin, HIGH); // 2)
        delay(50);
        digitalWrite(piezo_pin, LOW);
        delay(50);
        digitalWrite(piezo_pin, HIGH); // 3)
        delay(50);
        digitalWrite(piezo_pin, LOW);
        delay(50);
        digitalWrite(piezo_pin, HIGH); // 4)
        delay(50);
        digitalWrite(piezo_pin, LOW);
        delay(50);
        digitalWrite(piezo_pin, HIGH); // 5)
        delay(200);
        digitalWrite(piezo_pin, LOW);
    }

    void signalPreparation()
    {
        int interval = 5000;
        int duration = BOP; // interval time in milliseconds

        if (!arming::isPyroContinuity()) // if there is no pyro continuity signal with BEEP
        {
            duration = BEEP;
        }

        currentTime = millis();

        if (currentTime - previousTime >= interval && !buzzing)
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, HIGH);
            buzzing = true;
        }
        else if (currentTime - previousTime >= duration && buzzing)
        {
            digitalWrite(piezo_pin, LOW);
            buzzing = false;
        }
    }

    void signalDrogue()
    {
        int interval = 5000;
        int pause = BOP;
        int duration = BOP; // interval time in milliseconds
        int times_to_buzz = 2;
        currentTime = millis();

        if (currentTime - previousTime >= pause && !buzzing && times_buzzed < times_to_buzz) // if hasn't buzzed the allowed times and isn't currently buzzing - starts buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, HIGH);
            buzzing = true;
        }
        else if (currentTime - previousTime >= duration && buzzing) // if is currently buzzing and has buzzed more than allowed - ends buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, LOW);
            buzzing = false;
            times_buzzed++;
        }
        else if (currentTime - previousTime >= interval && !buzzing) // after every interval resets times the buzzr has buzzed
        {
            times_buzzed = 0;
        }
    }

    void signalMain()
    {
        int interval = 5000;
        int pause = BOP;
        int duration = BOP; // interval time in milliseconds
        int times_to_buzz = 3;
        currentTime = millis();

        if (currentTime - previousTime >= pause && !buzzing && times_buzzed < times_to_buzz) // if hasn't buzzed the allowed times and isn't currently buzzing - starts buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, HIGH);
            buzzing = true;
        }
        else if (currentTime - previousTime >= duration && buzzing) // if is currently buzzing and has buzzed more than allowed - ends buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, LOW);
            buzzing = false;
            times_buzzed++;
        }
        else if (currentTime - previousTime >= interval && !buzzing) // after every interval resets times the buzzr has buzzed
        {
            times_buzzed = 0;
        }
    }

    void signalDescent()
    {
        int interval = 5000;
        int pause = BOP;
        int duration = BOP; // interval time in milliseconds
        int times_to_buzz = 4;
        currentTime = millis();

        if (currentTime - previousTime >= pause && !buzzing && times_buzzed < times_to_buzz) // if hasn't buzzed the allowed times and isn't currently buzzing - starts buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, HIGH);
            buzzing = true;
        }
        else if (currentTime - previousTime >= duration && buzzing) // if is currently buzzing and has buzzed more than allowed - ends buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, LOW);
            buzzing = false;
            times_buzzed++;
        }
        else if (currentTime - previousTime >= interval && !buzzing) // after every interval resets times the buzzr has buzzed
        {
            times_buzzed = 0;
        }
    }

    void signalTouchdown()
    {
        int interval = 5000;
        int pause = BEEP;
        int duration = BEEP; // interval time in milliseconds
        int times_to_buzz = 4;
        currentTime = millis();

        if (currentTime - previousTime >= pause && !buzzing && times_buzzed < times_to_buzz) // if hasn't buzzed the allowed times and isn't currently buzzing - starts buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, HIGH);
            buzzing = true;
        }
        else if (currentTime - previousTime >= duration && buzzing) // if is currently buzzing and has buzzed more than allowed - ends buzz
        {
            previousTime = currentTime; // save the last time that buzzer was toggled
            digitalWrite(piezo_pin, LOW);
            buzzing = false;
            times_buzzed++;
        }
        else if (currentTime - previousTime >= interval && !buzzing) // after every interval resets times the buzzr has buzzed
        {
            times_buzzed = 0;
        }
    }

    void signalArmed()
    {
        digitalWrite(piezo_pin, HIGH); // 1)
        delay(50);
        digitalWrite(piezo_pin, LOW);
        delay(50);
        digitalWrite(piezo_pin, HIGH); // 2)
        delay(50);
        digitalWrite(piezo_pin, LOW);
        delay(50);
        digitalWrite(piezo_pin, HIGH); // 3)
        delay(50);
        digitalWrite(piezo_pin, LOW);
    }

    void signalBatteryVoltage()
    {
        int readingNum = arming::getBatteryVoltage() * 10;
        Serial.println("Singaling system health, reading: " + String(readingNum));
        int secondDigit = readingNum % 10;
        int firstDigit = (readingNum - secondDigit) / 10;

        // beep as many times as the first digit of battery voltage
        for (int i = 0; i < firstDigit; i++)
        {
            digitalWrite(piezo_pin, HIGH); // 1)
            delay(BOP);
            digitalWrite(piezo_pin, LOW);
            delay(BOP);
        }
        delay(500);
        // beep as many times as the second digit of battery voltage
        for (int i = 0; i < secondDigit; i++)
        {
            digitalWrite(piezo_pin, HIGH); // 1)
            delay(BOP);
            digitalWrite(piezo_pin, LOW);
            delay(BOP);
        }
        delay(500);
    }

    void signalWarningMainEjection()
    {
        digitalWrite(piezo_pin, HIGH);
        delay(3000);
        digitalWrite(piezo_pin, LOW);
        delay(1000);
    }

}