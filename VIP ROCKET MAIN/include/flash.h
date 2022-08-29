#pragma once

#include "FS.h"
#include <LITTLEFS.h>
#include "SD_card.h"
#include "sensor_data.h"
#include "eeprom_wrapper.h"

#define FORMAT_LITTLEFS_IF_FAILED true

unsigned long flash_time = millis();
unsigned long start_descent_time;

// to simplify the usage of the Flash header declared a different function - deleteFile - this serves as it's basis
void delete_File(fs::FS &fs, const char *path)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (fs.remove(path))
    {
        Serial.println("- file deleted");
    }
    else
    {
        Serial.println("- delete failed");
    }
}

template <unsigned long size>
struct Buffer
{
    int offset = 0;
    uint8_t buf[size];

    void push(uint8_t *value)
    {
        for (int i = 0; i < sizeof(value); i++)
            buf[i + offset] = value[i];
        offset += sizeof(value);
    };

    void clean()
    {
        offset = 0;
        for (int x = 0; x < size; x++)
            buf[x] = 0;
    }
};

template <unsigned long size>
struct StreamPipe
{
    char buf_out[size];
    int offset = 0;

    template <typename T>
    void getValue(T *value_p)
    {
        auto &value = *value_p;
        char char_buf[sizeof(T)];
        for (int i = 0; i < sizeof(T); i++)
            char_buf[i] = buf_out[i + offset];
        offset += sizeof(T);

        value = *(T *)char_buf; // Cast char array to type
    }
};

namespace flash
{

    void setup()
    {
        if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED))
        {
            Serial.println("LITTLEFS Mount Failed. Its possible you need to format the partition with LITTLEFS.format() just once");
            // LITTLEFS.format(); //comment this out - only for first time with testing flash
            return;
        }
        Serial.println("Flash setup");
    }

    void deleteFile(const char *path)
    {
        delete_File(LITTLEFS, path);
    }

    File openFile(String filepath = "/data.txt")
    {
        File file = LITTLEFS.open(filepath, FILE_APPEND);
        return file;
    }

    void closeFile(File file)
    {
        file.close();
    }

    File closeOpen(File file)
    {
        flash::closeFile(file);
        Serial.println("Closed and opened flash file");
        return flash::openFile();
    }

    float getTimeElapsed()
    {
        return millis() - flash_time;
    }

    //*Current data writing function
    int writeData(File file, sens_data::GpsData gpsData, sens_data::IMUData imuData, sens_data::BarometerData barData, sens_data::BatteryData batData, int wrState)
    {
        // counter for closing and opening file
        static int counter;

        // Flash timing
        float _time = flash::getTimeElapsed();
        auto time = (uint8_t *)(&_time);

        // GPS
        float _lat = gpsData.lat;   // 1.1
        float _lng = gpsData.lng;   // 1.2
        float _alt = gpsData.alt;   // 1.3
        float _sats = gpsData.sats; // 1.4

        auto lat = (uint8_t *)(&_lat);
        auto lng = (uint8_t *)(&_lng);
        auto alt = (uint8_t *)(&_alt);
        auto sats = (uint8_t *)(&_sats); // 1.4

        // Bar
        auto pressure = (uint8_t *)(&barData.pressure);             // 2.1
        auto altitude = (uint8_t *)(&barData.altitude);             // 2.2
        auto f_altitude = (uint8_t *)(&barData.f_altitude);         // 2.4
        auto f_velocity = (uint8_t *)(&barData.f_velocity);         // 2.5
        auto f_acceleration = (uint8_t *)(&barData.f_acceleration); // 2.6
        auto temperature = (uint8_t *)(&barData.temperature);       // 2.7

        // Bat
        auto bat1 = (uint8_t *)(&batData.bat1); // 3.1

        // Mag
        auto acc_x = (uint8_t *)(&imuData.acc_x); // 4.4
        auto acc_y = (uint8_t *)(&imuData.acc_y); // 4.5
        auto acc_z = (uint8_t *)(&imuData.acc_z); // 4.6
        auto gyr_x = (uint8_t *)(&imuData.gyr_x); // 4.1
        auto gyr_y = (uint8_t *)(&imuData.gyr_y); // 4.2
        auto gyr_z = (uint8_t *)(&imuData.gyr_z); // 4.3

        // Other
        float _rState = wrState;

        auto rState = (uint8_t *)(&_rState);

        auto const buf_size = sizeof(time) + sizeof(lat) + sizeof(lng) + sizeof(alt) + sizeof(sats) + sizeof(pressure) + sizeof(altitude) + sizeof(f_altitude) + sizeof(f_velocity) + sizeof(f_acceleration) + sizeof(temperature) + sizeof(bat1) + sizeof(acc_x) + sizeof(acc_y) + sizeof(acc_z) + sizeof(gyr_x) + sizeof(gyr_y) + sizeof(gyr_z) + sizeof(rState);
        Buffer<buf_size> buffer;

        buffer.push(time);

        buffer.push(lat);
        buffer.push(lng);
        buffer.push(alt);
        buffer.push(sats);

        buffer.push(pressure);
        buffer.push(altitude);
        buffer.push(f_altitude);
        buffer.push(f_velocity);
        buffer.push(f_acceleration);
        buffer.push(temperature);

        buffer.push(bat1);

        buffer.push(acc_x);
        buffer.push(acc_y);
        buffer.push(acc_z);
        buffer.push(gyr_x);
        buffer.push(gyr_y);
        buffer.push(gyr_z);

        buffer.push(rState);

        if (!file)
        {
            Serial.println("- failed to open file for writing");
            return 0;
        }
        file.write(buffer.buf, buf_size);

        counter++;
        return counter;
    }

    void readFlashVerbose(const char *path)
    {
        File file = LITTLEFS.open(path);
        // This is the size of reading
        auto const buf_size = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float);
        while (file.available())
        {

            StreamPipe<buf_size> stream;
            file.readBytes(stream.buf_out, buf_size);

            // Time
            float time = 0;
            stream.getValue<float>(&time);
            Serial.println("ellapsed time: " + String(time, 10));

            // GPS
            float lat = 0;
            stream.getValue<float>(&lat);
            Serial.println("lat: " + String(lat, 10));

            float lng = 0;
            stream.getValue<float>(&lng);
            Serial.println("lng: " + String(lng, 10));

            float alt = 0;
            stream.getValue<float>(&alt);
            Serial.println("alt: " + String(alt, 10));

            float sats = 0;
            stream.getValue<float>(&sats);
            Serial.println("sats: " + String(sats, 10));

            // Bar
            float pressure = 0;
            stream.getValue<float>(&pressure);
            Serial.println("pressure: " + String(pressure, 10));

            float altitude = 0;
            stream.getValue<float>(&altitude);
            Serial.println("altitude: " + String(altitude, 10));

            float f_altitude = 0;
            stream.getValue<float>(&f_altitude);
            Serial.println("f_altitude: " + String(f_altitude, 10));

            float f_velocity = 0;
            stream.getValue<float>(&f_velocity);
            Serial.println("f_velocity: " + String(f_velocity, 10));

            float f_acceleration = 0;
            stream.getValue<float>(&f_acceleration);
            Serial.println("f_acceleration: " + String(f_acceleration, 10));

            float temperature = 0;
            stream.getValue<float>(&temperature);
            Serial.println("temperature: " + String(temperature, 10));

            // Bat
            float bat1 = 0;
            stream.getValue<float>(&bat1);
            Serial.println("bat1: " + String(bat1, 10));

            // Mag
            float acc_x = 0;
            stream.getValue<float>(&acc_x);
            Serial.println("acc_x: " + String(acc_x, 10));

            float acc_y = 0;
            stream.getValue<float>(&acc_y);
            Serial.println("acc_y: " + String(acc_y, 10));

            float acc_z = 0;
            stream.getValue<float>(&acc_z);
            Serial.println("acc_z: " + String(acc_z, 10));

            float gyr_x = 0;
            stream.getValue<float>(&gyr_x);
            Serial.println("gyr_x: " + String(gyr_x, 10));

            float gyr_y = 0;
            stream.getValue<float>(&gyr_y);
            Serial.println("gyr_y: " + String(gyr_y, 10));

            float gyr_z = 0;
            stream.getValue<float>(&gyr_z);
            Serial.println("gyr_z: " + String(gyr_z, 10));

            // Other
            float rState = 0;
            stream.getValue<float>(&rState);
            Serial.println("rState: " + String(rState, 10));

            Serial.println();
        }
        file.close();
    }

    void readFlash(const char *path)
    {
        File file = LITTLEFS.open(path);
        // This is the size of reading
        auto const buf_size = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float);
        while (file.available())
        {

            StreamPipe<buf_size> stream;
            file.readBytes(stream.buf_out, buf_size);

            // Time
            float time = 0;
            stream.getValue<float>(&time);
            Serial.print(String(time, 4) + ",");

            // GPS
            float lat = 0;
            stream.getValue<float>(&lat);
            Serial.print(String(lat, 4) + ",");

            float lng = 0;
            stream.getValue<float>(&lng);
            Serial.print(String(lng, 4) + ",");

            float alt = 0;
            stream.getValue<float>(&alt);
            Serial.print(String(alt, 4) + ",");

            float sats = 0;
            stream.getValue<float>(&sats);
            Serial.print(String(sats, 4) + ",");

            // Bar
            float pressure = 0;
            stream.getValue<float>(&pressure);
            Serial.print(String(pressure, 4) + ",");

            float altitude = 0;
            stream.getValue<float>(&altitude);
            Serial.print(String(altitude, 4) + ",");

            float f_altitude = 0;
            stream.getValue<float>(&f_altitude);
            Serial.print(String(f_altitude, 4) + ",");

            float f_velocity = 0;
            stream.getValue<float>(&f_velocity);
            Serial.print(String(f_velocity, 4) + ",");

            float f_acceleration = 0;
            stream.getValue<float>(&f_acceleration);
            Serial.print(String(f_acceleration, 4) + ",");

            float temperature = 0;
            stream.getValue<float>(&temperature);
            Serial.print(String(temperature, 4) + ",");

            // Bat
            float bat1 = 0;
            stream.getValue<float>(&bat1);
            Serial.print(String(bat1, 4) + ",");

            // Mag
            float acc_x = 0;
            stream.getValue<float>(&acc_x);
            Serial.print(String(acc_x, 4) + ",");

            float acc_y = 0;
            stream.getValue<float>(&acc_y);
            Serial.print(String(acc_y, 4) + ",");

            float acc_z = 0;
            stream.getValue<float>(&acc_z);
            Serial.println(String(acc_z, 4));

            float gyr_x = 0;
            stream.getValue<float>(&gyr_x);
            Serial.print(String(gyr_x, 4) + ",");

            float gyr_y = 0;
            stream.getValue<float>(&gyr_y);
            Serial.print(String(gyr_y, 4) + ",");

            float gyr_z = 0;
            stream.getValue<float>(&gyr_z);
            Serial.print(String(gyr_z, 4) + ",");

            // Other
            float rState = 0;
            stream.getValue<float>(&rState);
            Serial.print(String(rState, 4) + ",");
        }
        file.close();
    }

    int dumpContentsToSD(const char *path, SD_File fileSD)
    {
        int count = 0;

        File file = LITTLEFS.open(path);
        // This is the size of reading
        auto const buf_size = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float);

        // write header to SD
        if (!SDcard::writeHeader(fileSD))
        {
            return 0;
        }

        Serial.println("Dumping flash contents to SD card");

        while (file.available())
        {

            StreamPipe<buf_size> stream;
            file.readBytes(stream.buf_out, buf_size);

            // Time
            float time = 0;
            stream.getValue<float>(&time);

            // GPS
            float lat = 0;
            stream.getValue<float>(&lat);

            float lng = 0;
            stream.getValue<float>(&lng);

            float alt = 0;
            stream.getValue<float>(&alt);

            float sats = 0;
            stream.getValue<float>(&sats);

            // Bar
            float pressure = 0;
            stream.getValue<float>(&pressure);

            float altitude = 0;
            stream.getValue<float>(&altitude);

            float f_altitude = 0;
            stream.getValue<float>(&f_altitude);

            float f_velocity = 0;
            stream.getValue<float>(&f_velocity);

            float f_acceleration = 0;
            stream.getValue<float>(&f_acceleration);

            float temperature = 0;
            stream.getValue<float>(&temperature);

            // Bat
            float bat1 = 0;
            stream.getValue<float>(&bat1);

            // Mag
            float acc_x = 0;
            stream.getValue<float>(&acc_x);

            float acc_y = 0;
            stream.getValue<float>(&acc_y);

            float acc_z = 0;
            stream.getValue<float>(&acc_z);

            float gyr_x = 0;
            stream.getValue<float>(&gyr_x);

            float gyr_y = 0;
            stream.getValue<float>(&gyr_y);

            float gyr_z = 0;
            stream.getValue<float>(&gyr_z);

            // Other
            float rState = 0;
            stream.getValue<float>(&rState);

            // write to SD card
            SDcard::writeData(fileSD, time, lat, lng, alt, sats, pressure, altitude, f_altitude, f_velocity, f_acceleration, temperature, bat1, acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, rState);

            count++;
            if (count % 500 == 0)
            {
                Serial.print(".");
            }
        }
        file.close();

        Serial.println("\nFinished dumping flash contents to SD card");
        return 1;
    }

    void lock()
    {
        eeprom::lockFlash();
    }

    void unlock()
    {
        eeprom::unlockFlash();
    }

    bool locked()
    {
        if (eeprom::lockedFlash())
        {
            Serial.println("Cannot delete flash - locked");
            return 1;
        }
        else
        {
            return 0;
        }
    }

    bool flashEnded(File file, int flash_duration = 30000)
    {
        static bool firstCall = true;
        static bool ended = false;
        if (firstCall)
        {
            start_descent_time = millis();
            firstCall = false;
        }
        if (!ended && millis() - start_descent_time > flash_duration)
        {
            ended = true;
            file.close();
            Serial.println("Flash Writing Ended");
        }
        return ended;
    }

    void testFileIO(File file, int multiplier)
    {
        float x_float = 0.0 + multiplier;
        float y_float = 17.1212332 + multiplier;
        float z_float = 99.9999 + multiplier;

        auto x = (uint8_t *)(&x_float);
        auto y = (uint8_t *)(&y_float);
        auto z = (uint8_t *)(&z_float);

        auto const buf_size = sizeof(x) + sizeof(y) + sizeof(z);
        Buffer<buf_size> buffer;

        buffer.push(x);
        buffer.push(y);
        buffer.push(z);

        if (!file)
        {
            Serial.println("Failed to open file for writing");
            return;
        }
        file.write(buffer.buf, buf_size);
    }
}