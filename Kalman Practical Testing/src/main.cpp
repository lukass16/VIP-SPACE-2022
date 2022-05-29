#include <Arduino.h>
#include "barometer_wrapper_MS5607.h"
#include "Kalman.h"
#include "kalman_flash.h"

unsigned long timer = millis(); // keeps track of delT and time
unsigned long t_init = millis(); // saves inital time

File file;

void setup()
{
	Serial.begin(115200);
	Serial.println("Kalman Filter Testing");

	barometer::setup();
	flash::setup();

	if(!flash::locked()) flash::deleteFile("/kalman.txt");

	file = flash::openFile();

	kalman::predict(); // make first prediction
	timer = millis();
	delay(50);
}

void loop()
{
	barometer::readSensor();

	// kalman implementation
	kalman::setDelT((millis() - timer)/1000);
	timer = millis();
	kalman::predict();
	kalman::updateBaro(barometer::alt);

	//printing info to Serial
	kalman::printFullInfoPosition(barometer::alt);

	//saving data to flash
	flash::writeKalmanState(file, barometer::alt, kalman::getKalmanPosition(), kalman::getKalmanVelocity(), kalman::getKalmanAcceleration(), kalman::getPositionKalmanGain(), kalman::getPositionUncertainty());

	if(millis() - t_init > 10000)
	{
		flash::closeFile(file);
		flash::readKalmanFlash("/kalman.txt");
		delay(1000000);
	}
	delay(50);
}

