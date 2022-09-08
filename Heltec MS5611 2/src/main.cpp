#include <Arduino.h>
#include <Wire.h>
#include <MS5x.h>
#include "barometer_wrapper_MS5607.h"

void setup()
{
	Serial.begin(115200);
	barometer::setup();
}

void loop()
{
	barometer::readSensor(false);
	barometer::printAll();
	delay(100);
}
