/*
    BasicMS5x.ino 
	
	Shows how to quickly connect to sensor using default address (0x77) and oversampling ratio (maximum oversampling)
    
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

void setup() {
	Serial.begin(115200);
	while(barometer.connect()>0) { // barometer.connect starts wire and attempts to connect to sensor
		Serial.println(F("Error connecting..."));
		delay(500);
	}
	Serial.println(F("Connected to Sensor"));
	delay(5);
}

void loop() {
	
	double pressure=0;
	double temperature=0;
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
	if (barometer.isReady()) { 
		temperature = barometer.GetTemp(); // Returns temperature in C
		pressure = barometer.GetPres(); // Returns pressure in Pascals
		Serial.print(F("The Temperature is: "));
		Serial.println(temperature);
		Serial.print(F("The Pressure is: "));
		Serial.println(pressure);
	}
}
