#include <Arduino.h>
#include "barometer_wrapper_MS5607.h"
#include "Kalman.h"
#include "kalman_flash.h"
#include "SD_card.h"

unsigned long timer = millis(); // keeps track of delT and time
unsigned long t_init = millis(); // saves inital time
float t_change = 0;

File file;
SD_File fileSD;
char sdfilename[50] = "kalman27.txt";

void setup()
{
	Serial.begin(115200);
	Serial.println("Kalman Filter Testing");

	barometer::setup();
	flash::setup();
	SDCard::setup();

	//open flash file
	if(!flash::locked()) flash::deleteFile("/kalman.txt");
	file = flash::openFile();

	//open SD file
	fileSD = SD.open(sdfilename, FILE_WRITES); //*Note: if the code is run multiple times it will append to this file and will create multiple header files
	fileSD.println("Time [s],Baro_P [m],Kalman_P [m],Kalman_V [m/s],Kalman_A [m/s^2],Kalman_Gain_P,Kalman_Uncertainty_P"); //print labels to file

	kalman::predict(); // make first prediction
	timer = millis();
	delay(50);
}

void loop()
{
	barometer::readSensor();

	// kalman implementation
	t_change = (millis() - timer) / 1000.0;
	kalman::setDelT(t_change);

	timer = millis();
	kalman::predict();
	kalman::updateBaro(barometer::alt);

	//printing info to Serial
	kalman::printFullInfoPosition(barometer::alt);

	//saving data to flash
	flash::writeKalmanState(file, barometer::alt, kalman::getKalmanPosition(), kalman::getKalmanVelocity(), kalman::getKalmanAcceleration(), kalman::getPositionKalmanGain(), kalman::getPositionUncertainty());

	//saving data to SD
	fileSD.println(String(millis()-t_init) + "," + String(barometer::alt, 2) + "," + String(kalman::getKalmanPosition(), 2) + "," + String(kalman::getKalmanVelocity(), 2) + "," + String(kalman::getKalmanAcceleration(), 2) + "," + String(kalman::getPositionKalmanGain(), 5) + "," + String(kalman::getPositionUncertainty(), 5) );

	if(millis() - t_init > 30000)
	{
		//flash close and read
		flash::closeFile(file);
		flash::readKalmanFlash("/kalman.txt");

		//SD close and read
		fileSD.close();
		fileSD = SD.open(sdfilename); //open file for reading
		SDCard::readSD(fileSD);

		Serial.println("Testing Finished");
		
		delay(1000000);
	}
	delay(50);
}

