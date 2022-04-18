#pragma once
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "sensor_data.h"
#include <Wire.h>

Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;


namespace imu
{
    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, tmp;

    void setup()
    {
        if (!mpu.begin(0x68))
        {
            Serial.println("Failed to find MPU6050 chip");
            while (1){delay(10);}
        }
        Serial.println("MPU6050 Found!");

        //set ranges
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    }

    void readSensor()
    {
        mpu.getEvent(&a, &g, &temp);

        //set acceleration values
        acc_x = a.acceleration.x;
        acc_y = a.acceleration.y;
        acc_z = a.acceleration.z;

        //set gyroscope values
        gyr_x = g.gyro.x;
        gyr_y = g.gyro.y;
        gyr_z = g.gyro.z;

        //set temperature value
        tmp = temp.temperature;
    }

    void readPrintAll()
    {
        mpu.getEvent(&a, &g, &temp);

        /* Print out the values */
        Serial.print("Acceleration X: ");
        Serial.print(a.acceleration.x);
        Serial.print(", Y: ");
        Serial.print(a.acceleration.y);
        Serial.print(", Z: ");
        Serial.print(a.acceleration.z);
        Serial.println(" m/s^2");

        Serial.print("Rotation X: ");
        Serial.print(g.gyro.x);
        Serial.print(", Y: ");
        Serial.print(g.gyro.y);
        Serial.print(", Z: ");
        Serial.print(g.gyro.z);
        Serial.println(" rad/s");

        Serial.print("Temperature: ");
        Serial.print(temp.temperature);
        Serial.println(" °C");
        Serial.print("\n");
    }

    sens_data::IMUData getIMUState()
    {
        sens_data::IMUData md;
        md.x = 42.0; //*has no magnetometer readings
        md.y = 42.0;
        md.z = 42.0;
        md.acc_x = acc_x;
        md.acc_y = acc_y;
        md.acc_z = acc_z;
        return md;
    }

}