#pragma once
#include <Arduino.h>

#define EEPROMCLEAR 37
#define SW1 38
#define SW2 39
#define PYROSENSE 36
#define FIREPYRO 32

namespace arming
{
    //*GPIO
    void setup()
    {
        pinMode(EEPROMCLEAR, INPUT);
        pinMode(SW1, INPUT);
        pinMode(SW2, INPUT);
        pinMode(PYROSENSE, INPUT);
        pinMode(FIREPYRO, OUTPUT);

        Serial.println("Arming setup complete!");
    }

    bool clearEEPROM()
    {
        if (digitalRead(EEPROMCLEAR) == 0)
        {
            Serial.println("Clearing EEPROM");
            return 1;
        }
        return 0;
    }

    bool armed()
    {
        if(digitalRead(SW1) == 0)
        {
            return 1;
        }
        return 0;
    }

    //*TIMERS
    //* 1) Apogee detection
    // creating a variable for timer detection of apogee
    volatile bool timerDetAp = 0;
    hw_timer_t *apogeeTimer = NULL;
    portMUX_TYPE apogeeTimerMux = portMUX_INITIALIZER_UNLOCKED;

    // creating the interrupt handling function - should be as short as possible
    void IRAM_ATTR onApogeeTimer()
    {
        portENTER_CRITICAL_ISR(&apogeeTimerMux);
        timerDetAp = 1;
        portEXIT_CRITICAL_ISR(&apogeeTimerMux);
    }

    void startApogeeTimer(int timerLength = 20000000) // timer length is given in microseconds
    {
        // initializing timer - setting the number of the timer, the value of the prescaler and stating that the counter should count up (true)
        apogeeTimer = timerBegin(0, 80, true);

        // binding the timer to a handling function
        timerAttachInterrupt(apogeeTimer, &onApogeeTimer, true);

        // specifying the counter value in which the timer interrupt will be generated and indicating that the timer should not automatically reload (false) upon generating the interrupt
        timerAlarmWrite(apogeeTimer, timerLength, false);

        // enabling the timer
        timerAlarmEnable(apogeeTimer);

        Serial.println("Apogee timer enabled");
    }

    bool timerDetectApogee()
    {
        if (timerDetAp)
        {
            Serial.println("Timer detected apogee!");
            return 1;
        }
        return 0;
    }

    //* 2) Main ejection detection
    // creating a variable for timer detection of main ejection
    volatile bool timerDetMainEject = 0;
    hw_timer_t *mainEjectTimer = NULL;
    portMUX_TYPE mainEjectTimerMux = portMUX_INITIALIZER_UNLOCKED;

    // creating the interrupt handling function - should be as short as possible
    void IRAM_ATTR onMainEjectTimer()
    {
        portENTER_CRITICAL_ISR(&mainEjectTimerMux);
        timerDetMainEject = 1;
        portEXIT_CRITICAL_ISR(&mainEjectTimerMux);
    }

    void startMainEjectionTimer(int timerLength = 30000000) // timer length is given in microseconds
    {
        // initializing timer - setting the number of the timer, the value of the prescaler and stating that the counter should count up (true)
        mainEjectTimer = timerBegin(1, 80, true);

        // binding the timer to a handling function
        timerAttachInterrupt(mainEjectTimer, &onMainEjectTimer, true);

        // specifying the counter value in which the timer interrupt will be generated and indicating that the timer should not automatically reload (false) upon generating the interrupt
        timerAlarmWrite(mainEjectTimer, timerLength, false);

        // enabling the timer
        timerAlarmEnable(mainEjectTimer);

        Serial.println("Main ejection timer enabled");
    }

    bool timerDetectMainEjection()
    {
        if (timerDetMainEject)
        {
            Serial.println("Timer detected main ejection!");
            return 1;
        }
        return 0;
    }

    //* 3) Touchdown detection
    // creating a variable for timer detection of touchdown
    volatile bool timerDetTouchdown = 0;
    hw_timer_t *touchdownTimer = NULL;
    portMUX_TYPE touchdownTimerMux = portMUX_INITIALIZER_UNLOCKED;

    // creating the interrupt handling function - should be as short as possible
    void IRAM_ATTR onTouchdownTimer()
    {
        portENTER_CRITICAL_ISR(&touchdownTimerMux);
        timerDetTouchdown = 1;
        portEXIT_CRITICAL_ISR(&touchdownTimerMux);
    }

    void startTouchdownTimer(int timerLength = 15000000) // timer length is given in microseconds
    {
        // initializing timer - setting the number of the timer, the value of the prescaler and stating that the counter should count up (true)
       touchdownTimer = timerBegin(2, 80, true);

        // binding the timer to a handling function
        timerAttachInterrupt(touchdownTimer, &onTouchdownTimer, true);

        // specifying the counter value in which the timer interrupt will be generated and indicating that the timer should not automatically reload (false) upon generating the interrupt
        timerAlarmWrite(touchdownTimer, timerLength, false);

        // enabling the timer
        timerAlarmEnable(touchdownTimer);

        Serial.println("Touchdown timer enabled");
    }

    bool timerDetectTouchdown()
    {
        if (timerDetTouchdown)
        {
            Serial.println("Timer detected Touchdown!");
            return 1;
        }
        return 0;
    }

}