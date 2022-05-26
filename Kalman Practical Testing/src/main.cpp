#include <Arduino.h>
#include "barometer_wrapper_MS5607.h"
#include "Kalman.h"

unsigned long timer = millis(); //keeps track of delT and time

void setup()
{
	Serial.begin(115200);
	Serial.println("Kalman Filter Testing");

	barometer::setup();
	
	timer = millis();
}

void loop()
{
	barometer::readSensor();

	//kalman implementation
	kalman::setDelT(millis()-timer);
	timer = millis();

	kalman::predict();
	kalman::updateBaro(barometer::alt);


	Serial.println("Barometer altitude: " + String(barometer::alt) + "\tKalman altitude: " + String(kalman::getKalmanPosition()));

	delay(100);
}