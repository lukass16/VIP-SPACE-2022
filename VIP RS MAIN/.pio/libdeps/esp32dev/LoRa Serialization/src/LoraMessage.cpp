#if ARDUINO >= 100
    #include "Arduino.h"
#endif
#include <stdlib.h>
#include "LoraMessage.h"
#include "LoraEncoder.h"

LoraMessage::LoraMessage() {
    _currentSize = 0;
    _buffer = (byte*) malloc(_currentSize);
}

LoraMessage::~LoraMessage() {
    free(_buffer);
}

LoraEncoder LoraMessage::_reallocBuffer(int delta) {
    void* temp = realloc(_buffer, (_currentSize + delta) * sizeof(byte));
    if (temp == NULL) {
        free(_buffer);
        printf("bad memory allocation!");
        while(true);
    } else {
        _buffer = (byte*) temp;
    }
    LoraEncoder encoder(_buffer + _currentSize);
    _currentSize += delta;
    return encoder;
}

LoraMessage& LoraMessage::addUnixtime(uint32_t unixtime) {
    _reallocBuffer(4).writeUnixtime(unixtime);
    return *this;
}


LoraMessage& LoraMessage::addUint16(uint16_t i) {
    _reallocBuffer(2).writeUint16(i);
    return *this;
}

LoraMessage& LoraMessage::addUint8(uint8_t i) {
    _reallocBuffer(1).writeUint8(i);
    return *this;
}

int LoraMessage::getLength() {
    return _currentSize;
}

byte* LoraMessage::getBytes() {
    return _buffer;
}
