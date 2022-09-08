//LORA ENDODER .cpp
#if ARDUINO >= 100
    #include "Arduino.h"
#endif
#include "LoraEncoder.h"

LoraEncoder::LoraEncoder(byte *buffer) {
  _buffer = buffer;
}

void LoraEncoder::_intToBytes(byte *buf, int32_t i, uint8_t byteSize) {
    for(uint8_t x = 0; x < byteSize; x++) {
        buf[x] = (byte) (i >> (x*8));
    }
}

int LoraEncoder::_BytesToNum(byte *buf, int32_t start, uint8_t size) {
    int num=0;
    for(uint8_t x = start; x < start+size; x++) {
        num = num + (int) (buf[x] << (x-start)*8);
    }
    return num;
}


void LoraEncoder::writeUnixtime(uint32_t unixtime) {
    _intToBytes(_buffer, unixtime, 4);
    _buffer += 4;
}



void LoraEncoder::writeMessage(double blat, double blng, double balt, int bsats, float bacc_x, float bacc_y, float bacc_z, float bpressure, float baltitude, float bf_altitude, float bf_velocity, float bbat1, int br_state, int bcounter) {
    
    //GPS
    int32_t lat = blat*10000;
    int32_t lng = blng*10000;
    int16_t alt = balt;
    int8_t sats = bsats;

    //IMU
    int32_t acc_x = bacc_x*100;
    int32_t acc_y = bacc_y*100;
    int32_t acc_z = bacc_z*100;
    
    //Barometer
    int16_t pres = bpressure;
    int32_t altitude = baltitude*10;
    int32_t f_altitude = bf_altitude*10;
    int32_t f_velocity = bf_velocity;

    //Battery
    int16_t bat1 = bbat1*10;

    //Other
    int8_t r_state = br_state;
    int16_t counter = bcounter; 
    

    //GPS data
    _intToBytes(_buffer, lat, 4);
    _intToBytes(_buffer + 4, lng, 4);
    _intToBytes(_buffer + 8, alt, 2);
    _intToBytes(_buffer + 10, sats, 1);

    //IMU data
    _intToBytes(_buffer + 11, acc_x, 4);
    _intToBytes(_buffer + 15, acc_y, 4);
    _intToBytes(_buffer + 19, acc_z, 4);
    
    //Barometer data
    _intToBytes(_buffer + 23, pres, 2);
    _intToBytes(_buffer + 25, altitude, 4);
    _intToBytes(_buffer + 29, f_altitude, 4);
    _intToBytes(_buffer + 33, f_velocity, 4);
  
    //Battery data
    _intToBytes(_buffer + 37, bat1, 2);

    //Other data
    _intToBytes(_buffer + 39, r_state, 1);
    _intToBytes(_buffer + 40, counter, 2);

}

void LoraEncoder::decodeMessage(byte *buf)
{
    int32_t s = 0; //buffer counter
    
    // GPS data
    double lat = _BytesToNum(buf, s, 4) / 10000.0;
    double lng = _BytesToNum(buf, s + 4, 4) / 10000.0;
    double alt = _BytesToNum(buf, s + 8, 2);
    int sats = _BytesToNum(buf, s + 10, 1);

    // IMU data
    float acc_x = _BytesToNum(buf, s + 11, 4) / 100.0;
    float acc_y = _BytesToNum(buf, s + 15, 4) / 100.0;
    float acc_z = _BytesToNum(buf, s + 19, 4) / 100.0;

    // Barometer data
    float pressure = _BytesToNum(buf, s + 23, 2);
    float altitude = _BytesToNum(buf, s + 25, 4) / 10.0;
    float f_altitude = _BytesToNum(buf, s + 29, 4) / 10.0;
    float f_velocity = _BytesToNum(buf, s + 33, 4);

    // Battery data
    float bat1 = _BytesToNum(buf, s + 37, 2) / 10.0;

    // Other data
    int current_rocket_state = _BytesToNum(buf, s + 39, 1);
    int counter = _BytesToNum(buf, s + 40, 2);

    Serial.printf("%7.4f,%7.4f,%5.0f,%2d,%4.2f,%4.2f,%4.2f,%5.0f,%6.1f,%6.1f,%3.0f,%2.1f,%1d,%4d\n", lat, lng, alt, sats, acc_x, acc_y, acc_z, pressure, altitude, f_altitude, f_velocity, bat1, current_rocket_state, counter);

}

void LoraEncoder::writeUint16(uint16_t i) {
    _intToBytes(_buffer, i, 2);
    _buffer += 2;
}

void LoraEncoder::writeUint8(uint8_t i) {
    _intToBytes(_buffer, i, 1);
    _buffer += 1;
}


