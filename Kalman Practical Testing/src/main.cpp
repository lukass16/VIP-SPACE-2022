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
char sdfilename[50] = "1.txt";

void setup()
{
	Serial.begin(115200);
	Serial.println("Kalman Filter Testing");
	
	barometer::setup();
	SDCard::setup();

	//open SD file
	fileSD = SD.open(sdfilename, FILE_WRITES); //*Note: if the code is run multiple times it will append to this file and will create multiple header files
	fileSD.println("Time [s],Time Change [s],Baro_P [m],Kalman_P [m],Kalman_V [m/s],Kalman_A [m/s^2],Kalman_Gain_P,Kalman_Uncertainty_P"); //print labels to file

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

	//saving data to SD
	fileSD.println(String(millis()-t_init) + "," + String(t_change, 4) + "," + String(barometer::alt, 2) + "," + String(kalman::getKalmanPosition(), 2) + "," + String(kalman::getKalmanVelocity(), 2) + "," + String(kalman::getKalmanAcceleration(), 2) + "," + String(kalman::getPositionKalmanGain(), 5) + "," + String(kalman::getPositionUncertainty(), 5) );

	if(millis() - t_init > 60000)
	{
		//SD close and read
		fileSD.close();

		Serial.println("Testing Finished");
		
		while(true);
	}
	
	delay(20);
}

