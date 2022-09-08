/*
    SetPolling.ino 
	
	Shows how to set a polling rate for your sensor so it is not being constantly asked for new values.
	Polling a sensor too quickly can lead to the device self heating resulting in inaccurate data.  If you observe
	temperature drift try decreasing the polling rate or lowering the oversampling ratio.  See the AdvancedMS5x Example 
	sketch for how to change the oversampling ratio.
    
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
	barometer.setDelay(1000); // barometer will wait 1 second before taking new temperature and pressure readings
}

void loop() {
	
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
	if (barometer.isReady()) { 
		temperature = barometer.GetTemp(); // Returns temperature in C
		pressure = barometer.GetPres(); // Returns pressure in Pascals
		
		// If a new temperature or pressure reading comes in then the sensor has been polled.
		// Print the new values and the time, in ms, between readings
		if ((temperature != prevTemperature) || (pressure != prevPressure)) {
			
			Serial.print(F("The New Temperature is: "));
			Serial.println(temperature);
			Serial.print(F("The New Pressure is: "));
			Serial.println(pressure);
			Serial.print(F("The Time between readings was: "));
			Serial.print(millis() - prevTime);
			Serial.println(F(" ms"));
			prevTime = millis();
			prevTemperature = temperature;
			prevPressure = pressure;
		}
	}
}
