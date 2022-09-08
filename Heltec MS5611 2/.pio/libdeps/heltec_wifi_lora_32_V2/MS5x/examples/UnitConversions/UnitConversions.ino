/*
    UnitConversions.ino 
	
	Shows how to set preferred temperature and pressure units
    
	Created 2021-05-25 
	By Matthew Bennett
	Modified -
	By -

    This file is part of arduino-MS5x.

    arduino-MS5x is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    arduino-MS5x is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with arduino-MS5x.  If not, see <http://www.gnu.org/licenses/>.

*/

//**************************************************************************************************************
//!
//! If you are using a MS5607 you need to open the header file and comment out line 24 (#define MS5611)
//! Failing to do this will result in the wrong math calculations being performed for temperature and pressure
//!
//**************************************************************************************************************

#include <Wire.h>
#include <MS5x.h>

MS5x barometer(&Wire);

uint32_t prevTime; // The time, in MS the device was last polled

double prevPressure=0; // The value of the pressure the last time the sensor was polled
double prevTemperature=0; // The value of the temperature the last time the sensor was polled

void setup() {
	Serial.begin(115200);
	while(barometer.connect()>0) { // barometer.connect starts wire and attempts to connect to sensor
		Serial.println(F("Error connecting..."));
		delay(500);
	}
	Serial.println(F("Connected to Sensor"));
		
	//These three lines will set the temperature units returned by GetTemp().

	//barometer.setTempC(); // Uncommenting this line will have GetTemp() return temperature in Celcius (default temperature units)
	//barometer.setTempF(); // Uncommenting this line will have GetTemp() return temperature in Fahrenheit 
	//barometer.setTempK(); // Uncommenting this line will have GetTemp() return temperature in Kelvin 
	
	//These three lines will set the pressure units returned by GetPress().
	
	//barometer.setPressHg(); // Uncommenting this line will have GetPress() return Pressure in Inches Mercury (Inches Hg)
	//barometer.setPressMbar(); // Uncommenting this line will have GetPress() return Pressure in Millibars 
	//barometer.setPressPa(); // Uncommenting this line will have GetPress() return Pressure in Pascals (default pressure units)
	
	barometer.setDelay(1000); // barometer will wait 1 second before taking new temperature and pressure readings
}

void loop() {
	
	double pressure = 0;
	double temperature = 0;
	
	/* In order to not have any delays used in code, checkUpdates cycles through sensor read process
	   Step 1: Ask for raw temperature calculation to be performed
	   Step 2: Once enough time has passed for calculation ask sensor to send results
	   Step 3: Ask for raw pressure calculation to be performed
	   Step 4: Once enough time has passed for calculation ask sensor to send results
       At this point checkUpdates returns true, but no new sensor readings will be performed until Readout function is called. */
	   barometer.checkUpdates();
	   
	if (barometer.isReady()) { 		
		temperature = barometer.GetTemp(); // Returns temperature in selected units
		pressure = barometer.GetPres(); // Returns pressure in selected units
		
		if ((temperature != prevTemperature) || (pressure != prevPressure)) {
			Serial.print(F("The Temperature is: "));
			Serial.println(temperature);
			Serial.print(F("The Pressure is: "));
			Serial.println(pressure);
			prevTime = millis();
			prevTemperature = temperature;
			prevPressure = pressure;
		}
	}
}
