#include <Arduino.h>
#include <Wire.h>
#include <MS5x.h>

MS5x barometer(&Wire);

void setup()
{
	Serial.begin(115200);
	while (barometer.connect() > 0)
	{ // barometer.connect starts wire and attempts to connect to sensor
		Serial.println(F("Error connecting..."));
		delay(500);
	}
	Serial.println(F("Connected to Sensor"));
	delay(5);
}

void loop()
{

	double pressure = 0;
	double temperature = 0;
	/*
	In order to not have any delays used in code, checkUpdates cycles through sensor read process
	Step 1: Ask for raw temperature calculation to be performed
	Step 2: Once enough time has passed for calculation ask sensor to send results
	Step 3: Ask for raw pressure calculation to be performed
	Step 4: Once enough time has passed for calculation ask sensor to send results
	At this point conversion preocess is complete and no new sensor readings will be performed until Readout function is called.
	*/
	barometer.checkUpdates();

	/*
	Updates Temperature and Pressure values for reading.  Until sensor is done with first set of calculations values will be zero.
	At max oversampling from the time the sensor is first connected to the time all conversions are complete is
	3 ms for booting + 10 ms for each step = ~43 ms + the amount of time it takes to run through a loop.

	Alternatively, prior to reading temperature and pressure data, check isReady().
	*/
	if (barometer.isReady())
	{
		temperature = barometer.GetTemp(); // Returns temperature in C
		pressure = barometer.GetPres();	   // Returns pressure in Pascals
		Serial.print(F("The Temperature is: "));
		Serial.println(temperature);
		Serial.print(F("The Pressure is: "));
		Serial.println(pressure);
	}
	delay(1000);
}
