#pragma once
#include <Arduino.h>
#include "mutex_wrapper.h"


namespace sens_data {

enum ROCKET_STATE {
    PREPERATION = 0, // Sensor checks 
    IN_FLIGHT = 1, // Safety pins removed - ready to launch or is already flying
    DESCENT = 2, // Parachute is realeased, rocket has reached apogee
    RECOVERY = 3 // Rocked is landed somewhere - need to find it 
};

struct GpsData 
{
    double lat = 0;
    double lng = 0;
    double alt = 0;
    int sats = 0;
};

struct MagenetometerData 
{
    float x = 0;
    float y = 0;
    float z = 0;
    // Acceleration
    float acc_x = 0;
    float acc_y = 0;
    float acc_z = 0;
};

struct BarometerData 
{
    float temperature = 0;
    float pressure = 0;
    float altitude = 0;
    float vert_velocity = 0;
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
        MagenetometerData magnetometerData;
        BarometerData barometerData;
        BatteryData batteryData;
        Mtx mtx;
    
    public:
        byte lora_message_id = 0;

        SensorData(){
            mtx.setup();
        };

        MagenetometerData getMagnetometerData()
        { 
            mtx.lock();
            MagenetometerData md = magnetometerData;
            mtx.unlock();
            return md;
        }
        void setMagnetometerData(MagenetometerData md)
        {
            mtx.lock();
            magnetometerData = md;
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
 