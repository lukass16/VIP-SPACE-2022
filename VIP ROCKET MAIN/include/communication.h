#pragma once
#include <Arduino.h>
#include <cstdio>
#include "sensor_data.h"
#include "lora_wrapper.h"
#include "thread_wrapper.h"
#include "SD_card.h"

namespace comms
{
    bool stopped = 0; //note - no protection used in case accessed from both threads simultaneously

    String serializeData();
    void loop(void *args);

    void setup(long frequency = 434.1E6) //434.1
    {
        lora::setup(frequency);
        s_thread::setup(loop);
    }

    void stop()
    {
        stopped = 1;
        Serial.println("Communication stopped!");
    }

    void resume()
    {
        stopped = 0;
        Serial.println("Communication resumed!");
    }

    // This is ran in a seperate thread
    void loop(void *args = NULL)
    {
        Serial.println("Thread initialized!");
        // Send lora every 0.4 secs
        while (true)
        {
            String serialized = comms::serializeData();
            Serial.println("Sent data: " + String(serialized));

            //*option 1 - Not encoded
            //lora::sendMessage(serialized, 1);

            //*option 2 - Encoded
            lora::encodeMessage(); 
            lora::sendEncodedMessage(1);

            delay(400);

            while (stopped) // stops the loop if necessary
            {
                delay(100);
            }
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
        int r_state = s_data.getRocketState();
        sprintf(outgoing, "%7.4f,%7.4f,%5.0f,%2d,%4.2f,%4.2f,%4.2f,%5.0f,%6.1f,%6.1f,%4.0f,%3.2f,%1d,%4d", gps.lat, gps.lng, gps.alt, gps.sats, imu.acc_x, imu.acc_y, imu.acc_z, bar.pressure, bar.altitude, bar.f_altitude, bar.f_velocity, bat.bat1, r_state, counter);
        counter++;
        return outgoing;
    }

}