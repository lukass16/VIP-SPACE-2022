#include <Arduino.h>
#include "MPU9250.h"

MPU9250 mpu; // You can also use MPU9255 as is

void setup()
{
	Serial.begin(115200);
	Serial.println("Testing alternate library with MPU9250.");
	Wire.begin(21, 22);
	delay(2000);
	mpu.setup(0x69);
}

void loop()
{
	if (mpu.update()) {
        Serial.print(mpu.getYaw()); Serial.print(", ");
        Serial.print(mpu.getPitch()); Serial.print(", ");
        Serial.println(mpu.getRoll());
    }
	delay(1000);
}