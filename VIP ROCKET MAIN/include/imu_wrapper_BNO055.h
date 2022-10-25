#pragma once
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "sensor_data.h"

// Possible vector values can be:
// - VECTOR_ACCELEROMETER - m/s^2
// - VECTOR_MAGNETOMETER  - uT
// - VECTOR_GYROSCOPE     - rad/s
// - VECTOR_EULER         - degrees
// - VECTOR_LINEARACCEL   - m/s^2
// - VECTOR_GRAVITY       - m/s^2

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);
sensors_event_t angVelocityData, accelerometerData;
sensors_event_t *gyrEvent, *accEvent;

namespace imu
{
    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, tmp;

    void setup()
    {
        Serial.println("Initializing BNO055");
        /* Initialise the sensor */
        if (!bno.begin(OPERATION_MODE_ACCGYRO))
        {
            /* There was a problem detecting the BNO055 ... check your connections */
            Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
            while (1);
        }
    }

    void readSensor()
    {
        bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
        bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
        accEvent = &accelerometerData;
        gyrEvent = &angVelocityData;
        acc_x = accEvent->acceleration.x / 9.807; //converting to [g]
        acc_y = accEvent->acceleration.y / 9.807;
        acc_z = accEvent->acceleration.z / 9.807;
        gyr_x = gyrEvent->gyro.x;
        gyr_y = gyrEvent->gyro.y;
        gyr_z = gyrEvent->gyro.z;
    }

    void printAll()
    {
        Serial.print("Acc X: " + String(acc_x, 2));
        Serial.print(", ");
        Serial.print("Acc Y: " + String(acc_y, 2));
        Serial.print(", ");
        Serial.print("Acc Z: " + String(acc_z, 2));

        Serial.print("\t");
        Serial.print("Gyro X: " + String(gyr_x, 2));
        Serial.print(", ");
        Serial.print("Gyro Y: " + String(gyr_y, 2));
        Serial.print(", ");
        Serial.println("Gyro Z: " + String(gyr_z, 2));
    }

    void printCalibrationStatus()
    {
        uint8_t system, gyro, accel, mag = 0;
        bno.getCalibration(&system, &gyro, &accel, &mag);
        Serial.println();
        Serial.print("Calibration: Sys=");
        Serial.print(system);
        Serial.print(" Gyro=");
        Serial.print(gyro);
        Serial.print(" Accel=");
        Serial.print(accel);
        Serial.print(" Mag=");
        Serial.println(mag);
    }

    void printMode()
    {
        Serial.print("Mode: ");
        Serial.println(bno.getMode());
    }

    void printSystemStatus()
    {
        uint8_t status, test_res, err;
        bno.getSystemStatus(&status, &test_res, &err);
        Serial.print("Status: ");
        Serial.print(status);
        Serial.print("\tSelf Test Result: ");
        Serial.print(test_res);
        Serial.print("\tSystem Error: ");
        Serial.println(err);
    }

    bool launchDetected(float threshold = 2.5, int times = 10) // threshold - threshold acceleration to be detected (in g), times - times for the threshold to be detected
    {
        static int counter = 0;
        if (acc_x > threshold)
        {
            counter++;
        }
        else
        {
            counter = 0;
        }
        if (counter > times)
        {
            Serial.println("Launch detected!");
            return true;
        }
        else
        {
            return false;
        }
    }

    sens_data::IMUData getIMUState()
    {
        sens_data::IMUData md;
        md.acc_x = acc_x;
        md.acc_y = acc_y;
        md.acc_z = acc_z;

        md.gyr_x = gyr_x;
        md.gyr_y = gyr_y;
        md.gyr_z = gyr_z;

        md.mag_x = 0.0; 
        md.mag_y = 0.0;
        md.mag_z = 0.0;

        return md;
    }

}
