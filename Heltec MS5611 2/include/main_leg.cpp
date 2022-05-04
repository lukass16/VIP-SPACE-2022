#include <Arduino.h>
#include <Wire.h>
#include <MS5x.h>

MS5x barometer(&Wire);
double seaLevelPressure = 0;
unsigned long start_t = 0;
int counter = 0;

void setup()
{
	Serial.begin(115200);

	//*Testing the elapsed time while the barometer gets good readings
	start_t = millis();

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
	counter++;
	double pressure = 0;
	double temperature = 0;
	double altitude = 0;

	barometer.checkUpdates();

	/*
	Updates Temperature and Pressure values for reading.  Until sensor is done with first set of calculations values will be zero.
	At max oversampling from the time the sensor is first connected to the time all conversions are complete is
	3 ms for booting + 10 ms for each step = ~43 ms + the amount of time it takes to run through a loop.

	Alternatively, prior to reading temperature and pressure data, check isReady().
	*/


	if (barometer.isReady())
	{
		// Calculate predicted seaLevel pressure based off a known altitude in meters
		if (seaLevelPressure == 0)
		{
			seaLevelPressure = barometer.getSeaLevel(10.5); //this functions also as the sea level setter for altitude calculations
			Serial.println("Sea level pressure set as: " + String(seaLevelPressure));
			Serial.println("Time ellapsed while barometer ready: " + String(millis()-start_t,5)); //*the delay is about 44 ms which is quite okay - it results from multiple loops while the sensor gets first readings
			Serial.println("Cycles ellapsed while barometer ready: " + String(counter));
			delay(100000);
		}

		temperature = barometer.GetTemp();	// Returns temperature in C
		pressure = barometer.GetPres();		// Returns pressure in Pascals
		altitude = barometer.getAltitude(); // Returns altitude in m - //*optionally temperature corrected
		Serial.print(F("The Temperature is: "));
		Serial.println(temperature);
		Serial.print(F("The Pressure is: "));
		Serial.println(pressure);
		Serial.print(F("The Altitude is: "));
		Serial.println(altitude);
	}
	//delay(1000);
}
