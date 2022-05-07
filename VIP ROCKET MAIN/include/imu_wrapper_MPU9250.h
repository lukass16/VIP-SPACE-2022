#pragma once
#include "MPU9250.h"
#include "sensor_data.h"

MPU9250 mpu;

namespace imu
{
    float acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, tmp;

    void setup()
    {
        mpu.setup(0x68); //! Note if the sensor can't connect check that the setup is forced in the MPU9250.h file
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
            Serial.print(mpu.getAccX());
            Serial.print(", ");
            Serial.print(mpu.getAccY());
            Serial.print(", ");
            Serial.print(mpu.getAccZ());

            Serial.print("\t");
            Serial.print(mpu.getGyroX());
            Serial.print(", ");
            Serial.print(mpu.getGyroY());
            Serial.print(", ");
            Serial.println(mpu.getGyroZ());
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

        md.mag_x = 42.0; //*has no magnetometer readings
        md.mag_y = 42.0;
        md.mag_z = 42.0;
        
        return md;
    }

}
