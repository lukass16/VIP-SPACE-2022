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



void LoraEncoder::writeMessage(float mag_x, float mag_y, float mag_z, float macc_x, float macc_y, float macc_z, double glat, double glng, double gps_alt, int sats, float temperature, float pressure, float bar_alt, float vert_velocity, float bat1, float bat2) {
    int16_t x = mag_x*100;
    int16_t y = mag_y*100;
    int16_t z = mag_z*100;
    int16_t acc_x = macc_x*100;
    int16_t acc_y = macc_y*100;
    int16_t acc_z = macc_z*100;

    glat = glat *10000;
    glng = glng*10000;
    int32_t lat = glat;
    int32_t lng = glng;
    int32_t alt = gps_alt;
    int8_t sat = sats;

    int16_t temp = temperature*10;
    int16_t pres = pressure;
    int32_t balt = bar_alt;
    int16_t vert_vel = vert_velocity*10;
    int8_t b1 = bat1*10;
    int8_t b2= bat2*10; 

    //magnetoemeter data
    _intToBytes(_buffer, x, 2);
    _intToBytes(_buffer + 2, y, 2);
    _intToBytes(_buffer + 4, z, 2);
    _intToBytes(_buffer + 6, acc_x, 2);
    _intToBytes(_buffer + 8, acc_y, 2);
    _intToBytes(_buffer + 10, acc_z, 2);
    _buffer += 12;
    
    //GPS data
    _intToBytes(_buffer, lat, 4);
    _intToBytes(_buffer + 4, lng, 4);
    _intToBytes(_buffer + 8, alt, 4);
    _intToBytes(_buffer + 12, sat, 1);
    _buffer += 13;

    //barometer data
    _intToBytes(_buffer, temp, 2);
    _intToBytes(_buffer + 2, pres, 2);
    _intToBytes(_buffer + 4, balt, 4);
    _intToBytes(_buffer + 8, vert_vel, 2);
    _buffer += 10;

    //rest
    _intToBytes(_buffer, b1, 1);
    _intToBytes(_buffer + 1, b2, 1);
    _buffer += 2;


}
void LoraEncoder::decodeMessage(byte *buf){
    int32_t s = 0;
    float x, y, z, acc_x, acc_y, acc_z;
    double lat, lng, gps_alt;
    float temperature, pressure, bar_alt, vert_velocity;
    float bat1, bat2;
    float sats, bs;

    //magnetoemeter data
    x = _BytesToNum(buf, s, 2);
    x= x/100;
    y = _BytesToNum(buf, s + 2, 2);
    y= y/100;
    z = _BytesToNum(buf, s + 4, 2);
    z= z/100;
    acc_x = _BytesToNum(buf, s + 6, 2);
    acc_x= acc_x/100;
    acc_y = _BytesToNum(buf, s + 8, 2);
    acc_y= acc_y/100;
    acc_z = _BytesToNum(buf, s + 10, 2);
    acc_z= acc_z/100;
    s = s+12;

    //GPS data
    lat = _BytesToNum(buf, s, 4);
    lat= lat/10000;
    lng = _BytesToNum(buf, s + 4, 4);
    lng = lng/ 10000;
    gps_alt = _BytesToNum(buf, s + 8, 4);
    sats = _BytesToNum(buf, s  +  12, 1);
    s = s + 13;

    //barometer data
    temperature = _BytesToNum(buf, s, 2);
    temperature= temperature/10;
    pressure = _BytesToNum(buf, s + 2, 2);
    bar_alt = _BytesToNum(buf, s + 4, 4);
    vert_velocity = _BytesToNum(buf, s + 8, 2);
    vert_velocity= vert_velocity/10;
    s = s + 10;

    //rest
    bat1 = _BytesToNum(buf, s, 1);
    bat1= bat1/10;
    bat2 = _BytesToNum(buf, s + 1, 1);
    bat2= bat2/10;
    


    Serial.printf("\n%.2f, %.2f,  %.2f,  %.2f,  %.2f,  %.2f, %.4f, %.4f, %.0f, %.0f, %.1f, %.0f, %.1f, %.2f, %.2f", x, y, z, acc_x, acc_y, acc_z, lat, lng, gps_alt, sats, temperature, pressure, vert_velocity, bat1, bat2);   
    
}
void LoraEncoder::writeUint16(uint16_t i) {
    _intToBytes(_buffer, i, 2);
    _buffer += 2;
}

void LoraEncoder::writeUint8(uint8_t i) {
    _intToBytes(_buffer, i, 1);
    _buffer += 1;
}


