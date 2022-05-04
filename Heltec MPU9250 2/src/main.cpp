#include <Arduino.h>
#include "MPU9250.h"
#include "imu_wrapper_MPU9250.h"


void setup()
{
	Serial.begin(115200);
	Serial.println("Testing alternate library with MPU9250.");
	Wire.begin(21, 22);

	imu::setup();
}

void loop()
{
	imu::readPrintAll();
	delay(1000);
}