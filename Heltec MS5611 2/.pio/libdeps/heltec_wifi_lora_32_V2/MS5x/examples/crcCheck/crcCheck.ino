/*
    crcCheck.ino 
	
	Shows how to perform a quick CRC check to ensure sensor was calibrated prior to leaving factory.
	I recommend that you perfrom a CRC check just once the first time you get a sensor
	What this does is do a CRC calculation based on the factory sensor callibration coefficents
	and then compares it to the CRC value stored in the sensor.  If they entered the data correctly
	then this function returns true.  Note, this doesn't guarentee that the sensor is calibrated correctly really.
	All it does is confirm they entered the CRC correctly based on the values they burned into sensors PROM.
	But if this comes back false then you definitely shouldn't trust it.
    
	
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

  // Compare calcuated CRC value against CRC value stored in sensor's PROM
	bool crcRes = barometer.checkCRC();
	Serial.print(F("Sensor CRC Check Results: "));
	crcRes ? Serial.println(F("CRC Passed")) : Serial.println(F("CRC Failed"));
	
	/* Confirm that the code to check the CRC values are accurate.  Sample PROM values were taken from
	TE-Connectivity tech note AN520.  If this returns anything except 0xB, then there is a problem with the checkCRC() function.
	*/
	uint8_t crcTest = barometer.CRCcodeTest();
	Serial.println(F("This function should return 0xB."));
	Serial.print(F("CRC code check returned: 0x"));
	Serial.println(crcTest, HEX);

  Serial.println();

  delay(1000);
}
