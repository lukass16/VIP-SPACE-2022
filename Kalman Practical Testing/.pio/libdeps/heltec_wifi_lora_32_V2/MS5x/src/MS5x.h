/*
    MS5x.h
	
	Header File for MS5x Library.  Provides for i2c communication with MS55xx, MS56xx, MS57xx, and MS58xx barometer sensors
    
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

#ifndef MS5x_h
#define MS5x_h

//#define MS5611

#include "Arduino.h"
#include <Wire.h>

// typical I2C-Address of chip
#define I2C_LOW 0x76
#define I2C_HIGH 0x77

// I2C commands of chip
#define MS5xxx_CMD_RESET	0x1E    // perform reset
#define MS5xxx_CMD_ADC_READ 0x00    // initiate read sequence
#define MS5xxx_CMD_ADC_CONV 0x40    // start conversion
#define MS5xxx_CMD_ADC_D1   0x00    // read ADC 1
#define MS5xxx_CMD_ADC_D2   0x10    // read ADC 2
#define MS5xxx_CMD_ADC_256  0x00    // set ADC oversampling ratio to 256
#define MS5xxx_CMD_ADC_512  0x02    // set ADC oversampling ratio to 512
#define MS5xxx_CMD_ADC_1024 0x04    // set ADC oversampling ratio to 1024
#define MS5xxx_CMD_ADC_2048 0x06    // set ADC oversampling ratio to 2048
#define MS5xxx_CMD_ADC_4096 0x08    // set ADC oversampling ratio to 4096
#define MS5xxx_CMD_PROM_RD  0xA0    // initiate readout of PROM registers

class MS5x
{
  protected:
	// Functions
	void ReadProm(); // Reads factory scaling data, only needs to be read once at startup (or again after sensor reset) 
	void Readout(); // Converts Temperature and Pressure readings to desired units
	
	uint8_t Calc_CRC4(uint16_t nprom[]); // Calculate expected CRC4
	uint8_t Read_CRC4(); // Read CRC4 from sensor
	uint8_t send_cmd(uint8_t aCMD); // Sends a conversion command or read request to sensor
	
	uint32_t read_adc(); // Converts D1 and D2 into scaled values.
    	
	// Variables
	bool _INITALIZED = false; // True after all data has been calculated, false when first connected or reset.
	
	int8_t _I2CADDR = I2C_HIGH; // I2C address of sensor, default is I2C_HIGH (0x77)
	int8_t _READSTEP = 0; // Used to handle polling for sensor
	uint8_t _SAMPLERATIO = 0x08; // Oversampling ratio, default is 4096.  Set using setSamples(aCMD) where aCMD is value as defined in definition area above (MS5xxx_CMD_ADC_####)
	uint8_t _TTYPE = 0; // Temperature units, 0 = C, 1 = F, 2 = K
	uint8_t _PTYPE = 0; // Pressure units, 0 = Pa, 1 = mbar, 2 = inch Hg
	
	uint16_t _COEFFICENTS[8]; // Calibration Coefficents
	int16_t _POFFSET = 0; // Pressure offset value where 1000 = 1000 Pascals (10.00 millibars)
	int16_t _TOFFSET = 0; // Temperature offset value where 2000 = 20.00°C
	
	uint32_t _D1=0; // Unscaled pressure reading
	uint32_t _D2=0; // Unscaled temperature reading
	uint32_t _POLLDELAY=0; // Time to wait (in ms) between polling sensor
	uint32_t _PREVSTEPTIME = 0; // Time (in millis) of previous step.
	uint32_t _READDELAY = 10; // Time in MS to wait between read requests from the device, default is minimum required delay for maximum oversampling
	uint32_t _READDELAYPREV = 10; // Holds read delay value when preforming Reset.
	
	double _PRESS = 0; // converted and scaled pressure reading
	double _SEALEVELPRESS = 0; // Calculated Sea Level Pressure
	double _TEMP = 0; // converted and scaled temperature reading
	
	TwoWire *_Wire;
	
  public:
	// Functions
	MS5x(TwoWire *aWire); // Constructor function
	
	void checkUpdates(); // Uses timers to step through the process of converting temperature and pressure readings
	void reset(); // Resets the sensor
	void setI2Caddr(int8_t aAddr); // Sets I2C address
	void setDelay(uint32_t pollDelay); // Sets default poll Time
	void setPOffset(int16_t pOffset); // Sets offset for pressure reading where 1000 = 1000 Pascals (10.00 millibars)
	void setTOffset(int16_t tOffset); // Sets offset for temperature reading where 2000 = 20.00°C
	void setPressHg(); // Sets Pressure reading to Inches Murcury
	void setPressMbar(); // Sets Pressure reading to millbars 
	void setPressPa(); // Sets Pressure reading to Pascals (default)
	void setTempC(); // Sets Temperature reading to Celcius (default)
	void setTempF(); // Sets Temperature reading to Fahrenheit
	void setTempK(); // Sets Temperature reading to Kelvin
	void setSamples(uint8_t sampleRatio); // Sets the oversampling ratio the sensor will use to calculate pressure and temperature readings
		
	uint8_t connect(uint8_t sampleRatio = MS5xxx_CMD_ADC_4096); // Connects to device and sets oversampling ratio.  Default is max oversampling
	uint8_t CRCcodeTest(); // Check validity of CRC, not working currently

	bool checkCRC(); // Returns true if calculated CRC matches CRC stored in sensor's PROM
	bool isReady(); // Returns true after temperature and pressure readings have been completed the first time
	
	double getAltitude(bool temperatureCorrected = false); // Calculates Altitude off of difference between seaLevelPressure and current pressure.  Optionally performs temperature correction.
	double getSeaLevel(double altitude); // Calculate pressure at seaLevel based on current pressure reading and assumed current altitude
	double GetPres(); // Get latest Pressure reading from sensor
	double GetTemp(); // Get latest Temperature reading from sensor
	
	// Variables
};

#endif
