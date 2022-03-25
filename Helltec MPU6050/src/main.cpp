#include <Arduino.h>
#include "accelerometer.h"



void setup(void)
{
	Serial.begin(115200);
	Serial.println("Adafruit MPU6050 test!");
	accelerometer::setup();
	
}

void loop()
{
	accelerometer::readPrintAll();
	delay(300);
}