#pragma once
#include <Arduino.h>
#include "mutex_wrapper.h"


namespace sens_data {

enum ROCKET_STATE {
    PREPERATION = 0, // Sensor checks 
    MAIN = 1, //  is already flying
    DROGUE = 2, // Parachute is realeased, rocket has reached apogee
    DESCENT = 3 // Rocked is landing/has landed somewhere - need to find it 
};

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
    float f_velocity = 0; //vertical velocity
    float f_acceleration = 0; //vertical acceleration
};

struct BatteryData
{
    float bat1 = 0;
    float bat2 = 0;
    int bs = 0;
};


class SensorData
{
    private:
        ROCKET_STATE current_rocket_state;
        GpsData gpsData;
        IMUData imuData;
        BarometerData barometerData;
        BatteryData batteryData;
        Mtx mtx;
    
    public:
        byte lora_message_id = 0;

        SensorData(){
            mtx.setup();
        };

        IMUData getIMUData()
        { 
            mtx.lock();
            IMUData md = imuData;
            mtx.unlock();
            return md;
        }
        void setIMUData(IMUData md)
        {
            mtx.lock();
            imuData = md;
            mtx.unlock();
        }

        GpsData getGpsData()
        { 
            mtx.lock();
            GpsData gd = gpsData;
            mtx.unlock();
            return gd;
        }
        void setGpsData(GpsData gd) 
        {
            mtx.lock();
            gpsData = gd;
            mtx.unlock();
        }

        BarometerData getBarometerData()
        {
            mtx.lock();
            BarometerData bd = barometerData;
            mtx.unlock();
            return bd;
        }
        void setBarometerData(BarometerData bd)
        {
            mtx.lock();
            barometerData = bd;
            mtx.unlock();
        }

        BatteryData getBatteryData()
        {
            mtx.lock();
            BatteryData btd = batteryData;
            mtx.unlock();
            return btd;
        }
        void setBatteryData(BatteryData btd)
        {
            mtx.lock();
            batteryData = btd;
            mtx.unlock();
        }
};

}

extern sens_data::SensorData s_data;
 