#pragma once
#include <Arduino.h>
#include <cstdio>
#include "sensor_data.h"
#include "lora_wrapper.h"
#include "thread_wrapper.h"

using namespace std;

namespace comms
{
    String serializeData();
    void loop(void *args);

    void setup(long frequency = 433E6)
    {
        lora::setup(frequency);
        s_thread::setup(loop);  
    }

    // This is ran in a seperate thread
    void loop(void *args = NULL)
    {
        Serial.println("Thread initialized!");
        // Send lora every 0.4 secs
        while (true)
        {
            String serialized = comms::serializeData();
            lora::sendMessage(serialized, s_data.lora_message_id);
            Serial.print("Lora (msg id: ");
            Serial.print(s_data.lora_message_id);
            Serial.print(") sent: ");
            Serial.println(serialized);
            s_data.lora_message_id++;
            delay(400);
        }
    }

    String serializeData()
    {
        char outgoing[80];
        static int counter = 0;
        sens_data::GpsData gps = s_data.getGpsData();
        sens_data::IMUData imu = s_data.getIMUData();
        sens_data::BarometerData bar = s_data.getBarometerData();
        sens_data::BatteryData bat = s_data.getBatteryData();
        sprintf(outgoing, "%7.4f,%7.4f,%5.0f,%2d,%5.2f,%5.2f,%5.2f,%3.1f,%6.1f,%3.0f,%1d,%4d", gps.lat, gps.lng, gps.alt, gps.sats, imu.acc_x, imu.acc_y, imu.acc_z, bar.temperature, bar.altitude, bar.vert_velocity, bat.bs, counter); //*imu sends acceleration in place of magnetic field strength
        counter++;
        return outgoing;
    }

}