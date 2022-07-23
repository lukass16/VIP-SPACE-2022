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
    int16_t acc_x = bacc_x*100;
    int16_t acc_y = bacc_y*100;
    int16_t acc_z = bacc_z*100;
    
    //Barometer
    int16_t pres = bpressure;
    int32_t altitude = baltitude*10;
    int32_t f_altitude = bf_altitude*10;
    int16_t f_velocity = bf_velocity;

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
    _intToBytes(_buffer + 11, acc_x, 2);
    _intToBytes(_buffer + 13, acc_y, 2);
    _intToBytes(_buffer + 15, acc_z, 2);
    
    //Barometer data
    _intToBytes(_buffer + 17, pres, 2);
    _intToBytes(_buffer + 19, altitude, 4);
    _intToBytes(_buffer + 23, f_altitude, 4);
    _intToBytes(_buffer + 27, f_velocity, 2);
  
    //Battery data
    _intToBytes(_buffer + 29, bat1, 2);

    //Other data
    _intToBytes(_buffer + 31, r_state, 1);
    _intToBytes(_buffer + 32, counter, 2);

}

void LoraEncoder::decodeMessage(byte *buf){
    /*
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
    */
}
void LoraEncoder::writeUint16(uint16_t i) {
    _intToBytes(_buffer, i, 2);
    _buffer += 2;
}

void LoraEncoder::writeUint8(uint8_t i) {
    _intToBytes(_buffer, i, 1);
    _buffer += 1;
}


