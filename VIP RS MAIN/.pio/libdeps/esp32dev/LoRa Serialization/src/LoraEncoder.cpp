// LORA ENDODER .cpp
#if ARDUINO >= 100
#include "Arduino.h"
#endif
#include "LoraEncoder.h"
#include "sensor_data.h"

LoraEncoder::LoraEncoder(byte *buffer)
{
    _buffer = buffer;
}

void LoraEncoder::_intToBytes(byte *buf, int32_t i, uint8_t byteSize)
{
    for (uint8_t x = 0; x < byteSize; x++)
    {
        buf[x] = (byte)(i >> (x * 8));
    }
}

int LoraEncoder::_BytesToNum(byte *buf, int32_t start, uint8_t size)
{
    int num = 0;
    for (uint8_t x = start; x < start + size; x++)
    {
        num = num + (int)(buf[x] << (x - start) * 8);
    }
    return num;
}

void LoraEncoder::writeUnixtime(uint32_t unixtime)
{
    _intToBytes(_buffer, unixtime, 4);
    _buffer += 4;
}

void LoraEncoder::writeMessage(double blat, double blng, double balt, int bsats, float bacc_x, float bacc_y, float bacc_z, float bpressure, float baltitude, float bf_altitude, float bf_velocity, float bbat1, int br_state, int bcounter)
{

    // GPS
    int32_t lat = blat * 10000;
    int32_t lng = blng * 10000;
    int16_t alt = balt;
    int8_t sats = bsats;

    // IMU
    int16_t acc_x = bacc_x * 100;
    int16_t acc_y = bacc_y * 100;
    int16_t acc_z = bacc_z * 100;

    // Barometer
    int16_t pres = bpressure;
    int32_t altitude = baltitude * 10;
    int32_t f_altitude = bf_altitude * 10;
    int16_t f_velocity = bf_velocity;

    // Battery
    int16_t bat1 = bbat1 * 10;

    // Other
    int8_t r_state = br_state;
    int16_t counter = bcounter;

    // GPS data
    _intToBytes(_buffer, lat, 4);
    _intToBytes(_buffer + 4, lng, 4);
    _intToBytes(_buffer + 8, alt, 2);
    _intToBytes(_buffer + 10, sats, 1);

    // IMU data
    _intToBytes(_buffer + 11, acc_x, 2);
    _intToBytes(_buffer + 13, acc_y, 2);
    _intToBytes(_buffer + 15, acc_z, 2);

    // Barometer data
    _intToBytes(_buffer + 17, pres, 2);
    _intToBytes(_buffer + 19, altitude, 4);
    _intToBytes(_buffer + 23, f_altitude, 4);
    _intToBytes(_buffer + 27, f_velocity, 2);

    // Battery data
    _intToBytes(_buffer + 29, bat1, 2);

    // Other data
    _intToBytes(_buffer + 31, r_state, 1);
    _intToBytes(_buffer + 32, counter, 2);
}

sens_data::SensorData LoraEncoder::decodeMessage(byte *buf)
{
    int32_t s = 0; //buffer counter

    sens_data::SensorData s_data; //data object
    

    // GPS data
    s_data.gpsData.lat = _BytesToNum(buf, s, 4) / 10000;
    s_data.gpsData.lng = _BytesToNum(buf, s + 4, 4) / 10000;
    s_data.gpsData.alt = _BytesToNum(buf, s + 8, 2);
    s_data.gpsData.sats = _BytesToNum(buf, s + 10, 1);

    // IMU data
    s_data.imuData.acc_x = _BytesToNum(buf, s + 11, 2) / 100;
    s_data.imuData.acc_y = _BytesToNum(buf, s + 13, 2) / 100;
    s_data.imuData.acc_z = _BytesToNum(buf, s + 15, 2) / 100;

    // Barometer data
    s_data.barometerData.pressure = _BytesToNum(buf, s + 17, 2);
    s_data.barometerData.altitude = _BytesToNum(buf, s + 19, 4) / 10;
    s_data.barometerData.f_altitude = _BytesToNum(buf, s + 23, 4) / 10;
    s_data.barometerData.f_velocity = _BytesToNum(buf, s + 27, 2);

    // Battery data
    s_data.batteryData.bat1 = _BytesToNum(buf, s + 29, 2) / 10;

    // Other data
    s_data.current_rocket_state = _BytesToNum(buf, s + 31, 1);
    s_data.counter = _BytesToNum(buf, s + 32, 2);

    Serial.printf("%7.4f,%7.4f,%5.0f,%2d,%4.2f,%4.2f,%4.2f,%5.0f,%6.1f,%6.1f,%3.0f,%2.1f,%1d,%4d", s_data.gpsData.lat, s_data.gpsData.lng, s_data.gpsData.alt, s_data.gpsData.sats,  s_data.imuData.acc_x,  s_data.imuData.acc_y,  s_data.imuData.acc_z, s_data.barometerData.pressure, s_data.barometerData.altitude, s_data.barometerData.f_altitude, s_data.barometerData.f_velocity, s_data.batteryData.bat1, s_data.current_rocket_state, s_data.counter);

    return s_data;
}

void LoraEncoder::writeUint16(uint16_t i)
{
    _intToBytes(_buffer, i, 2);
    _buffer += 2;
}

void LoraEncoder::writeUint8(uint8_t i)
{
    _intToBytes(_buffer, i, 1);
    _buffer += 1;
}
