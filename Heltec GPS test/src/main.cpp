#include <Arduino.h>
#include "gps_wrapper.h"

void setup()
{
	Serial.begin(9600);
	Serial.println("Gps test");
	gps::setup();

}

void loop()
{
	gps::readGps();
}