#pragma once
#include <Arduino.h>

namespace sens_data
{

    struct GpsData
    {
        double lat = 0;
        double lng = 0;
        double alt = 0;
        int sats = 0;
    };

    struct IMUData
    {
        // Acceleration
        float acc_x = 0;
        float acc_y = 0;
        float acc_z = 0;
        // Rotation
        float gyr_x = 0;
        float gyr_y = 0;
        float gyr_z = 0;
        // Magnetic field
        float mag_x = 0;
        float mag_y = 0;
        float mag_z = 0;
    };

    struct BarometerData
    {
        float temperature = 0;
        float pressure = 0;
        float altitude = 0;
        float f_altitude = 0;
        float f_velocity = 0;     // vertical velocity
        float f_acceleration = 0; // vertical acceleration
    };

    struct BatteryData
    {
        float bat1 = 0;
        float bat2 = 0;
        int bs = 0;
    };

    class SensorData
    {
    public:
        int current_rocket_state = 0;
        int counter;
        GpsData gpsData;
        IMUData imuData;
        BarometerData barometerData;
        BatteryData batteryData;
    };

}
