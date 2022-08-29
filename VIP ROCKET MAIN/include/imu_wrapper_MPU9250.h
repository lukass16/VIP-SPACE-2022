#pragma once
#include "MPU9250.h"
#include "sensor_data.h"

MPU9250 mpu;

namespace imu
{
    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, tmp;

    void setup()
    {
        mpu.setup(0x68); // Note if the sensor can't connect check that the setup is forced in the MPU9250.h file
    }

    void calibrateAccelAndGyro()
    {
        // sensor must be stationary during calibration
        mpu.calibrateAccelGyro();
    }

    void readSensor()
    {
        //*Magnetometer readings not available when forced (not included in reading)
        if (mpu.update())
        {
            //*Get readings from accelerometer + gyro
            acc_x = mpu.getAccX();
            acc_y = mpu.getAccY();
            acc_z = mpu.getAccZ();

            gyr_x = mpu.getGyroX();
            gyr_y = mpu.getGyroY();
            gyr_z = mpu.getGyroZ();
        }
    }

    void readPrintAll()
    {
        if (mpu.update())
        {
            //*Get readings from accelerometer + gyro
            Serial.print("Acc X: " + String(mpu.getAccX(), 2));
            Serial.print(", ");
            Serial.print("Acc Y: " + String(mpu.getAccY(), 2));
            Serial.print(", ");
            Serial.print("Acc Z: " + String(mpu.getAccZ(), 2));

            Serial.print("\t");
            Serial.print("Gyro X: " + String(mpu.getGyroX(), 2));
            Serial.print(", ");
            Serial.print("Gyro Y: " + String(mpu.getGyroY(), 2));
            Serial.print(", ");
            Serial.println("Gyro Z: " + String(mpu.getGyroZ(), 2));
        }
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

    bool launchDetected(float threshold = -1, int times = 10) //threshold - threshold acceleration to be detected (in g), times - times for the threshold to be detected
    {
        static int counter = 0;
        if (acc_y < threshold)
        {
            counter++;
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

        md.mag_x = 0.0; //*has no magnetometer readings
        md.mag_y = 0.0;
        md.mag_z = 0.0;

        return md;
    }

}
