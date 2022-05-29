#pragma once

#include "FS.h"
#include <LITTLEFS.h>
#include "EEPROM.h"

#define FORMAT_LITTLEFS_IF_FAILED true

unsigned long flash_time = millis();

//to simplify the usage of the Flash header declared a different function - deleteFile - this serves as it's basis
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
            //LITTLEFS.format(); //comment this out - only for first time with testing flash
            return;
        }
        Serial.println("SPIFFS-like write file to new path and delete it w/folders");
    }

    void deleteFile(const char *path)
    {
        delete_File(LITTLEFS, path);
    }

    File openFile(String filepath = "/kalman.txt")
    {
        File file = LITTLEFS.open(filepath, FILE_APPEND);
        return file;
    }

    float getTimeElapsed() //*Check overflow
    {
        return millis()-flash_time;
    }

    void writeKalmanState(File file, float b_alt, float p, float v, float a, float k_p, float p_p)
    {
        //Flash timing
        float _time = flash::getTimeElapsed();
        auto time = (uint8_t *)(&_time);

        //Barometer reading
        float _alt = b_alt;
        auto alt = (uint8_t *)(&_alt); 

        //Kalman Filter Data
        float _p = p; //1.1
        float _v = v; //1.2
        float _a = a; //1.3
        float _k_p = k_p; //1.4
        float _p_p = p_p; //1.1

        auto pos = (uint8_t *)(&_p);
        auto vel = (uint8_t *)(&_v);
        auto acc = (uint8_t *)(&_a);
        auto k_gain = (uint8_t *)(&_k_p); 
        auto p_unc = (uint8_t *)(&_p_p); 

        auto const buf_size = sizeof(time) + sizeof(alt) + sizeof(pos) + sizeof(vel) + sizeof(acc) + sizeof(k_gain) + sizeof(p_unc);
        Buffer<buf_size> buffer;

        buffer.push(time);
        buffer.push(alt);
        buffer.push(pos);
        buffer.push(vel);
        buffer.push(acc);
        buffer.push(k_gain);
        buffer.push(p_unc);

        if (!file) Serial.println("- failed to open file for writing");
        file.write(buffer.buf, buf_size);
        
    }

    void readKalmanFlash(const char *path)
    {
        File file = LITTLEFS.open(path);
        //This is the size of reading
        auto const buf_size = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float);
        
        while (file.available())
        {

            StreamPipe<buf_size> stream;
            file.readBytes(stream.buf_out, buf_size);
 
            //Time
            float time = 0;
            stream.getValue<float>(&time);
            Serial.print(String(time, 4) + ",");

            //GPS
            float alt = 0;
            stream.getValue<float>(&alt);
            Serial.print(String(alt, 4) + ",");

            float pos = 0;
            stream.getValue<float>(&pos);
            Serial.print(String(pos, 4) + ",");

            float vel = 0;
            stream.getValue<float>(&vel);
            Serial.print(String(vel, 4) + ",");

            float acc = 0;
            stream.getValue<float>(&acc);
            Serial.print(String(acc, 4) + ",");

            float k_gain = 0;
            stream.getValue<float>(&k_gain);
            Serial.print(String(k_gain, 5) + ",");

            float p_unc = 0;
            stream.getValue<float>(&p_unc);
            Serial.println(String(p_unc, 4) + ",");

        }

        file.close();
    }

    void lock()
    {
        Serial.println("Flash locked");
        EEPROM.writeFloat(40, 5); //5 chosen as arbitrary value in case this address is used for more fucntionality (file names)
        EEPROM.commit();
    }

    void unlock()
    {
        Serial.println("Flash unlocked");
        EEPROM.writeFloat(40, 0);
        EEPROM.commit();
    }

    bool locked()
    {
        if (EEPROM.readFloat(40) == 5)
        {
            Serial.println("Cannot delete flash - locked");
            return 1;
        }
        else {return 0;}
    }

    void closeFile(File file)
    {
        file.close();
    }
}