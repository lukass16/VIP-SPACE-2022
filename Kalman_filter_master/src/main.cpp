#include <Arduino.h>
#include <Kalman1.h>
#include <heltec.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SDcard.h>

double p_baro = 0;
float t_sensor = 0, t_prev_sensor = 0;
char buffer[40] = "";

void deserializeData(char buffer[])
{
	sscanf(buffer, "%lf,%f", &p_baro, &t_sensor);
}
void nextMeasurement()
{
	t_prev_sensor = t_sensor; //current timestep becomes last timestep
	String line = SDcard::readline();
	line.toCharArray(buffer, 40);
	deserializeData(buffer);
	Serial.println("P_baro: " + String(p_baro) + " T_sensor: " + String(t_sensor, 5));
}

void saveState(double baro)
{
	SDcard::saveStateToFile(baro);
}

File readFile, writeFile;

void setup()
{
	Serial.begin(115200);
	Serial.println("Heltec test");
	SDcard::setup();

	SDcard::openFile();
	SDcard::openStateFile();
	nextMeasurement();
}

void loop()
{
	nextMeasurement();
	
	//*Kalman stuff
	saveState(KALMAN(p_baro));

	if(!SDcard::fileAvailable())
	{
		SDcard::closeFile();
		SDcard::closeStateFile();
		while(1);
	}
}
