#include <Arduino.h>
#include <Kalman.h>
#include <SDcard.h>
#include <Wire.h>

float p_baro = 0;
float t_sensor = 0, t_prev_sensor = 0;
char buffer[40] = "";

void deserializeData(char buffer[])
{
	sscanf(buffer, "%f,%f", &p_baro, &t_sensor);
}

void nextMeasurement()
{
	t_prev_sensor = t_sensor; //current timestep becomes last timestep
	String line = SDcard::readline();
	line.toCharArray(buffer, 40);
	deserializeData(buffer);
	Serial.println("P_baro: " + String(p_baro) + " T_sensor: " + String(t_sensor, 5));
}

void saveState()
{
	SDcard::saveStateToFile(kalman::getKalmanPosition(), kalman::getKalmanVelocity(), kalman::getKalmanAcceleration());
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Kalman Filter tests");
	SDcard::setup();
	
	SDcard::openFile();
	SDcard::openStateFile();
	nextMeasurement();
	

	//kalman::predict();
	//SDcard::closeFile();
	//SDcard::setStateFile("/filtered9.txt");
}

void loop()
{
	nextMeasurement();
	//delay((t_sensor - t_prev_sensor)*1000); //in ms //*Instead replaced by setDelT() to account for the discrepencies in time due to calculations
	kalman::setDelT(t_sensor - t_prev_sensor); //*Provide the predict() function with the appropriate change in time
	kalman::predict();

	kalman::updateBaro(p_baro); // this doesn't necessarily have to be done right before prediction (extrapolation) but can be just done every time a new reading is available

	//kalman::printKalmanState();
	//kalman::printPostionUncertainty();
	saveState();

	//TODO - fix large error at start
	if(!SDcard::fileAvailable())
	{
		SDcard::closeFile();
		SDcard::closeStateFile();
		while(1);
	}
}