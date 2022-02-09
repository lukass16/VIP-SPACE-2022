#pragma once

#include "Arduino.h"
#include "MPU9250.h"

//note: this wrapper also includes the EEPROM functionality

namespace magnetometer {
    
    // an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
    MPU9250 IMU(Wire, 0x68);
    int status;

    // declaring variables for maximum and minimum values in all of the axes
    float maxx = 71.72;
    float maxy = 69.32;
    float maxz = 7.16;
    float minx = -33.59;
    float miny = -34.93;
    float minz = -98.09;

    // declaring variables for current x y z values in the loop
    float cx = 0;
    float cy = 0;
    float cz = 0;

    // declaring variable for time up to last change and interval
    // since the last change of value, time in loop and time when loop started
    unsigned long time_up_to_change = 0;
    unsigned long interval_since_change = 0;
    unsigned long time_in_loop = 0;
    unsigned long start_time = 0;

    // declaring variable for the detectable interval since last change of max min value in milliseconds
    int interval = 15000; //magnetometer calibration interval

    // declaring variables for offsets
    float offset_x = 1;
    float offset_y = 2;
    float offset_z = 3;

    // declaring variables for average deltas
    float avg_delta = 0;
    float avg_delta_x = 0;
    float avg_delta_y = 0;
    float avg_delta_z = 0;

    // declaring variables for x y z scale factors
    float scale_x = 4;
    float scale_y = 5;
    float scale_z = 6;

    // declaring variables for corrected x y z values
    float cor_x = 99;
    float cor_y = 99;
    float cor_z = 99;

    // declaring variables for current accelerometer x y z values in the loop
    float acc_x = 0, acc_y = 0, acc_z = 0;

    // declaring variables for current gyroscope x y z values in the loop
    float gyr_x = 0, gyr_y = 0, gyr_z = 0;

    //creating variables for timer and launch
    float detAcc = 19.6; //accelerometer launch detection threshold values [m/s^2]
    int countAcc = 0;

    //creating variables for apogee detection and protection
    bool buzzApogee = 0, buzzApogeeOn = 0;
    float apogeeDetVal = -2; //magnetometer apogee detection threshold value

    //creating a variable for timer detection of apogee
    volatile bool timerDetAp = 0;

    //creating a pointer to a variable of type hw_timer_t
    hw_timer_t *timer = NULL;

    //declaring a variable of type portMUX_TYPE, which will be used to take care of the synchronization between the main loop and the ISR, when modifying a shared variable
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

    //creating the interrupt handling function - should be as short as possible
    //The interrupt handling routine should have the IRAM_ATTR attribute, in order for the compiler to place the code in IRAM
    void IRAM_ATTR onTimer()
    {
        //since the variable is shared with the main loop it will be modified inside a critical section, declared with the portENTER_CRITICAL_ISR and portEXIT_CRITICAL_ISR macros
        portENTER_CRITICAL_ISR(&timerMux);
        timerDetAp = 1;
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    //creating a variable for timer Lockout
    volatile bool armed = 0;

    //creating a pointer to a variable of type hw_timer_t
    hw_timer_t *timerLockout = NULL;

    //declaring a variable of type portMUX_TYPE, which will be used to take care of the synchronization between the main loop and the ISR, when modifying a shared variable
    portMUX_TYPE timerLockoutMux = portMUX_INITIALIZER_UNLOCKED;

    //creating the interrupt handling function - should be as short as possible
    //The interrupt handling routine should have the IRAM_ATTR attribute, in order for the compiler to place the code in IRAM
    void IRAM_ATTR onLockoutTimer()
    {
        //since the variable is shared with the main loop it will be modified inside a critical section, declared with the portENTER_CRITICAL_ISR and portEXIT_CRITICAL_ISR macros
        portENTER_CRITICAL_ISR(&timerMux);
        armed = 1;
        portEXIT_CRITICAL_ISR(&timerMux);
    }

    void startApogeeTimer(int timerLength)
    {
        //initializing timer - setting the number of the timer, the value of the prescaler and stating that the counter should count up (true)
        timer = timerBegin(0, 80, true);

        //binding the timer to a handling function
        timerAttachInterrupt(timer, &onTimer, true);

        //specifying the counter value in which the timer interrupt will be generated and indicating that the timer should not automatically reload (false) upon generating the interrupt
        timerAlarmWrite(timer, timerLength, false);

        //enabling the timer
        timerAlarmEnable(timer);

        Serial.println("Timer enabled");
    }

    void startLockoutTimer(int timerLength)
    {
        //initializing timer - setting the number of the timer, the value of the prescaler and stating that the counter should count up (true)
        timerLockout = timerBegin(2, 80, true);

        //binding the timer to a handling function
        timerAttachInterrupt(timerLockout, &onLockoutTimer, true);

        //specifying the counter value in which the timer interrupt will be generated and indicating that the timer should not automatically reload (false) upon generating the interrupt
        timerAlarmWrite(timerLockout, timerLength, false);

        //enabling the timer
        timerAlarmEnable(timerLockout);

        Serial.println("Lockout timer started");
    }

    bool timerDetectApogee()
    {
        if (timerDetAp)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    float getAccX()
    {
        acc_x = IMU.getAccelX_mss();
        return acc_x;
    }

    float getAccY()
    {
        acc_y = IMU.getAccelY_mss();
        return acc_y;
    }

    float getAccZ()
    {
        acc_z = IMU.getAccelZ_mss();
        return acc_z;
    }

    float getGyroX()
    {
        gyr_x = IMU.getGyroX_rads();
        return gyr_x;
    }

    float getGyroY()
    {
        gyr_y = IMU.getGyroY_rads();
        return gyr_y;
    }

    float getGyroZ()
    {
        gyr_z = IMU.getGyroZ_rads();
        return gyr_z;
    }

    void getMagValues()
    {
        cor_x = (IMU.getMagX_uT() - offset_x) * scale_x;
        cor_y = (IMU.getMagY_uT() - offset_y) * scale_y;
        cor_z = (IMU.getMagZ_uT() - offset_z) * scale_z;
    }

    void getAccelValues()
    {
        getAccX();
        getAccY();
        getAccZ();
    }

    void getGyroValues()
    {
        getGyroX();
        getGyroY();
        getGyroZ();
    }

    void displayAcceleration()
    {
        Serial.println("Accel_X: " + String(acc_x));
        Serial.println("Accel_Y: " + String(acc_y));
        Serial.println("Accel_Z: " + String(acc_z));
        Serial.println("");
    }

    int calibrateAccel()
    {
        return IMU.calibrateAccel();
    }

    void displayCor()
    {
        Serial.print("offset_x ");
        Serial.println(offset_x);
        Serial.print("scale_x ");
        Serial.println(scale_x);
        Serial.print("offset_y ");
        Serial.println(offset_y);
        Serial.print("scale_y ");
        Serial.println(scale_y);
        Serial.print("offset_z ");
        Serial.println(offset_z);
        Serial.print("scale_z ");
        Serial.println(scale_z);
    }

    void setup()
    {
        status = IMU.begin();
        if (status < 0)
        {
            Serial.println("IMU initialization unsuccessful");
            Serial.println("Check IMU wiring or try cycling power");
            Serial.print("Status: ");
            Serial.println(status);
            while (true)
                ;
        }

        Serial.println("Magnetometer ready");
    }

    boolean isApogee(float field_val = cor_y)
    {
        if(!armed)
        {
            return 0;
        }
        else
        {
            return field_val <= apogeeDetVal;
        }
    }

    double getAngle() {
        return atan(sqrt(pow(IMU.getAccelX_mss(), 2) + pow(IMU.getAccelZ_mss(), 2) ) / IMU.getAccelY_mss())*57.2958;
    }

    void printCalibratingValues()
    {
        Serial.print("\t max X: ");
        Serial.print(maxx);
        Serial.print("\t max Y: ");
        Serial.print(maxy);
        Serial.print("\t max Z: ");
        Serial.print(maxz);
        Serial.println("");
        Serial.print("\t min X: ");
        Serial.print(minx);
        Serial.print("\t min Y: ");
        Serial.print(miny);
        Serial.print("\t min Z: ");
        Serial.println(minz);
    }

    void calibrate(boolean skip = false)
    {

        if (skip)
        {
            Serial.println("Calibration SKIPPED!!!");
            return;
        }

        Serial.println("Calibrating magnetometer");

        //finding max and min values in all of the axes
        //assigning start time for the loop
        start_time = millis();
        //the loop will end after an interval of 30 seconds in which the min or max value hasn't changed in any of the axes
        while (interval_since_change < interval)
        {
            IMU.readSensor();
            // get current mag values

            cx = IMU.getMagX_uT();
            cy = IMU.getMagY_uT();
            cz = IMU.getMagZ_uT();

            //updating time in loop
            time_in_loop = millis() - start_time;

            //get min max x values
            if (cx > maxx)
            {
                maxx = cx;
                time_up_to_change = time_in_loop;
            }
            else if (cx < minx)
            {
                minx = cx;
                time_up_to_change = time_in_loop;
            }

            //get min max y values
            if (cy > maxy)
            {
                maxy = cy;
                time_up_to_change = time_in_loop;
            }
            else if (cy < miny)
            {
                miny = cy;
                time_up_to_change = time_in_loop;
            }

            //get min max z values
            if (cz > maxz)
            {
                maxz = cz;
                time_up_to_change = time_in_loop;
            }
            else if (cz < minz)
            {
                minz = cz;
                time_up_to_change = time_in_loop;
            }

            printCalibratingValues();

            //updating the time interval since last change of value
            interval_since_change = time_in_loop - time_up_to_change;

            Serial.println(interval_since_change);
        }

        //getting offset values
        offset_x = (maxx + minx) / 2;
        offset_y = (maxy + miny) / 2;
        offset_z = (maxz + minz) / 2;

        //getting average deltas for x y z axes
        avg_delta_x = (maxx - minx) / 2;
        avg_delta_y = (maxy - miny) / 2;
        avg_delta_z = (maxz - minz) / 2;
        //getting the average delta
        avg_delta = (avg_delta_x + avg_delta_y + avg_delta_z) / 3;

        //getting the scale factors
        scale_x = avg_delta / avg_delta_x;
        scale_y = avg_delta / avg_delta_y;
        scale_z = avg_delta / avg_delta_z;

        Serial.println("Calibration END !!!");
    }

    void displayData()
    {
        Serial.print("\t");
        Serial.print(cor_x);
        Serial.print("\t");
        Serial.print(cor_y);
        Serial.print("\t");
        Serial.print(cor_z);
        Serial.println("");
    }

    void enableBuzzApogee()
    {
        buzzApogee = 1;
    }

    void disableBuzzApogee()
    {
        buzzApogee = 0;
    }


    void arm(bool lockout = false)
    {
        if(lockout)
        {
            startLockoutTimer(2500000); //magnetometer apogee lockout timer (timer length)
        }
        else
        {
            armed = 1;
        }
    }

    void readMagnetometer()
    {
        // read the sensor
        IMU.readSensor();

        getMagValues();
        getAccelValues();
    }

}