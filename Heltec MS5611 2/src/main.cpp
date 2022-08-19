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
	for(int i = 0; i < 10; i++)
	{
		barometer::readSensor(false);
		barometer::printAll();
		delay(100);
	}

	Serial.println("Sampled sea level pressure: " + String(barometer::sampleSeaLevel()));
}
