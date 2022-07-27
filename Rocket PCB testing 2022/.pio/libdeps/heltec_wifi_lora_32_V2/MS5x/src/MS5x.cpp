/*
    MS5x.cpp 
	
	Main File for MS5x Library.  Provides for i2c communication with MS55xx, MS56xx, MS57xx, and MS58xx barometer sensors
    
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

#include "MS5x.h"
#include <math.h>

//******************************************************** 
//! @brief constructor sets ic2address to default of 0x77
//!
//! @scope Public
//! 
//! @return N/A
//********************************************************
MS5x::MS5x(TwoWire *aWire) : _I2CADDR(I2C_HIGH) {
	_Wire=aWire;
}

//******************************************************** 
//! @brief reads factory calibration data, technically only needs to be called once
//! after sensor starts up or resets (or when calculating CRC?)
//!
//! @scope Private
//! 
//! @return void
//********************************************************
void MS5x::ReadProm() {
	// There are 8 Coefficent values stored in PROM each 16 bits (two bytes) long so for each one
	// two reads are required.  
	for(uint8_t i=0;i<8;i++) 
	{
	    _COEFFICENTS[i]=0x0000; // Clear out any value already in the local variable
	    send_cmd(MS5xxx_CMD_PROM_RD+2*i); // Each coefficent takes 2 addresses, multiply by two time loop counter to get next coefficent 
	    uint8_t ret = _Wire->requestFrom(_I2CADDR, 2); // We are asking for 2 bytes

		if (ret == 2) {
			unsigned int c = _Wire->read();
			_COEFFICENTS[i] = (c << 8); // First byte returned is high byte, shift it 1 byte left and read next byte
			c = _Wire->read();
			_COEFFICENTS[i] += c;
		} else {
			// Maybe add some error checking here?
		}
	}
	
}

//******************************************************** 
//! @brief Sends sensor a reset command 
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::reset() {
	// On device reset clear updated flag if set and reset device
	_READSTEP = -1;
	_INITALIZED = false;
	_READDELAYPREV = _READDELAY;
	_READDELAY = 3; // After a reset wait 3 ms before executing any commands.
	send_cmd(MS5xxx_CMD_RESET);
	_PREVSTEPTIME = millis();
	checkUpdates(); //Force checkUpdates to ensure proper order of code execution
}

//******************************************************** 
//! @brief sets I2C address 
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setI2Caddr(int8_t aAddr) {
	_I2CADDR=aAddr;
}

//******************************************************** 
//! @brief sets time, in MS, to wait between polling device.
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setDelay(uint32_t pollDelay) {
	_POLLDELAY = pollDelay;
}

//******************************************************** 
//! @brief sets Pressure offset
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setPOffset(int16_t pOffset) {
	_POFFSET = pOffset;
}

//******************************************************** 
//! @brief sets Temperature offset 
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setTOffset(int16_t tOffset) {
	_TOFFSET = tOffset;
}

//******************************************************** 
//! @brief sets Pressure reading to be in Pascals (default)
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setPressPa() {
	if (_PRESS != 0 and _PTYPE != 0) { // Only convert if a new type is specified and Pressure has a value
		switch (_PTYPE) {
			case 0: break; // Just including this for safety
			case 1: // mbar to Pa
				_PRESS *= 100.0f; 
				_SEALEVELPRESS *= 100.0f; 
				break; 
			case 2: // inches Hg to Pa
				_PRESS *= 3386.3886667;
				_SEALEVELPRESS *= 3386.3886667;
				break; 
		}
	}
	_PTYPE = 0;
}

//******************************************************** 
//! @brief sets Pressure reading to be in millbars 
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setPressMbar() {
	if (_PRESS != 0 and _PTYPE != 1) { // Only convert if a new type is specified and Pressure has a value
		switch (_PTYPE) {
			case 0: // Pa to mbar
				_PRESS *= .01f;
				_SEALEVELPRESS *= .01f;
				break; 
			case 1: break; // Just including this for safety
			case 2: // inches Hg to mbar
				_PRESS *= 33.863886667;
				_SEALEVELPRESS *= 33.863886667;
				break; 
		}
	}
	_PTYPE = 1;
}

//******************************************************** 
//! @brief sets Pressure reading to be in Inches Mecury
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setPressHg() {
	if (_PRESS != 0 and _PTYPE != 2) { // Only convert if a new type is specified and Pressure has a value
		switch (_PTYPE) {
			case 0: // Pa to inches Hg
				_PRESS /= 3386.3886667;
				_SEALEVELPRESS /= 3386.3886667;
				break; 
			case 1: // mbar to inches Hg 
				_PRESS /= 33.863886667;
				_SEALEVELPRESS /= 33.863886667;
				break;
			case 2: break; // Just including this for safety
		}
	}
	_PTYPE = 2;
}

//******************************************************** 
//! @brief sets Temperature reading to be in Celcius (default)
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setTempC() {
	if (_TEMP != 0 and _TTYPE != 0) { // Only convert if a new type is specified and temp has a value
		switch (_TTYPE) { // Convert from existing units to C
			case 0: break; // Just including this for safety
			case 1: 
				_TEMP = (_TEMP - 32.0f) * 5.0f / 9.0f;
				break; // F to C
			case 2: 
				_TEMP -= 273.15f;
				break; // K to C
		}
	}
	_TTYPE = 0;
}

//******************************************************** 
//! @brief sets Temperature reading to be in Fahrenheit
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setTempF(){
	if (_TEMP != 0 and _TTYPE != 1) { // Only convert if a new type is specified and temp has a value
		switch (_TTYPE) { // Convert from existing units to C
			case 0:
				_TEMP = _TEMP * 9.0f/5.0f +32.0f;
				break; // C to F
			case 1: break; // Just including this for safety
			case 2:
				_TEMP = _TEMP * 9.0f / 5.0f - 459.67;
				break; // K to F
		}
	}
	_TTYPE = 1;
}

//******************************************************** 
//! @brief sets Temperature reading to be in Kelvin
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setTempK() {
	if (_TEMP != 0 and _TTYPE != 2) { // Only convert if a new type is specified and temp has a value
		switch (_TTYPE) { // Convert from existing units to C
			case 0:
				_TEMP += 273.15f;
				break; // C to K
			case 1:
				_TEMP = (_TEMP + 459.67)
				* 5.0f/9.0f; break; // F to K
			case 2: break; // Just including this for safety
		}
	}
	_TTYPE = 2;
}

//******************************************************** 
//! @brief Sets the desired oversampling ratio along with minimum required delays rates
//! before conversions can be read from sensor.
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::setSamples(uint8_t sampleRatio) {
	switch (sampleRatio)
  {
    case MS5xxx_CMD_ADC_256 : 
		_READDELAY=1;
		_SAMPLERATIO = 0x00;
		break;
    case MS5xxx_CMD_ADC_512 : 
		_READDELAY=3;
		_SAMPLERATIO = 0x02;
		break;
    case MS5xxx_CMD_ADC_1024: 
		_READDELAY=4;
		_SAMPLERATIO = 0x02;
		break;
    case MS5xxx_CMD_ADC_2048: 
		_READDELAY=6;
		_SAMPLERATIO = 0x06;
		break;
    case MS5xxx_CMD_ADC_4096: 
		_READDELAY=10;
		_SAMPLERATIO = 0x08;
		break;
	}
}

//******************************************************** 
//! @brief Calculates CRC based on 16 bits reserved for manufacturer,
//! 6 calibration coefficents, and final CRC address.
//! when called from checkCRC(), it can be used to confirm that sensor had
//! CRC correctly set to calibration data in factory.
//! Please reference TE Connectivity tech note AN520 for more information
//!
//! @scope Private
//! 
//! @return uint8_t (CRC calculation)
//********************************************************
uint8_t MS5x::Calc_CRC4(uint16_t n_prom[])
{
    uint8_t n_bit;
    int16_t cnt; // Simple counter
    uint16_t n_rem; // CRC remainder
    uint16_t crc_read; // Original value of the CRC

    n_rem = 0x00;

    crc_read = n_prom[7];	// Save original value of CRC
    n_prom[7] = (0xFF00 & (n_prom[7])); // CRC byte is replaced by 0
    for ( cnt = 0; cnt < 16; cnt++ ) // Operation is performed on bytes
    {
		// choose LSB or MSB
        if ( cnt % 2 == 1 ) n_rem ^= (uint8_t)((n_prom[cnt>>1]) & 0x00FF);
        else n_rem ^= (uint8_t)(n_prom[cnt>>1]>>8);

        for (n_bit=8; n_bit>0; n_bit--)
        {
            if (n_rem & (0x8000))
            {
            	n_rem=(n_rem<<1) ^ 0x3000;
            }
            else
            {
                n_rem=(n_rem<<1);
            }
        }
    }
    
    n_rem = (0x000F & (n_rem >> 12)); // final 4-bit remainder is CRC code
	n_prom[7] = crc_read; // restore the CRC_READ to it original place
    return (n_rem ^ 0x00);
}

//******************************************************** 
//! @brief intializes connection to sensor and sets up defauls values
//!
//! @scope Public
//! 
//! @return byte (i2c status)
//********************************************************
uint8_t MS5x::connect(uint8_t sampleRatio) {
	_PREVSTEPTIME = millis(); // Initialize timer
	setSamples(sampleRatio); // If a sample ratio has been provided, set sample ratio and delay
	_Wire->begin();
	_Wire->beginTransmission(_I2CADDR);
	uint8_t ret=_Wire->endTransmission();
	reset(); // After starup, reset the device, and read PROM
	return ret;
}

//******************************************************** 
//! @brief Used to make sure the Calc_CRC4 function was working correctly
//! serves no function in actual program beyond showing an example of how CRC check works
//!
//! @scope Public
//! 
//! @return uint8_t (result of Calc_CRC4 using assumed values for sensor calibration coefficents
//! if my Calc_CRC4 code is correct, this funciton should always return 0xB)
//********************************************************
uint8_t MS5x::CRCcodeTest(){
	uint16_t nprom[] = {0x3132,0x3334,0x3536,0x3738,0x3940,0x4142,0x4344,0x4500}; //expected output is 0xB
	uint8_t crc = Calc_CRC4(nprom); //expected output is 0xB
	//ReadProm();
	return crc;
}

//******************************************************** 
//! @brief Reads CRC value from sensor
//!
//! @scope Private
//! 
//! @return uint8_t (Stored CRC value in sensor)
//********************************************************
uint8_t MS5x::Read_CRC4()
{

    uint16_t crc_read = ( 0x000F & ( _COEFFICENTS[ 7 ] ) );
    return ( crc_read );
}

//******************************************************** 
//! @brief sends command to sensor
//!
//! @scope Private
//! 
//! @return byte (i2c status)
//********************************************************
uint8_t MS5x::send_cmd(uint8_t aCMD)
{
  _Wire->beginTransmission(_I2CADDR);
  _Wire->write(aCMD);
  uint8_t ret=_Wire->endTransmission();
  return ret;
}

//******************************************************** 
//! @brief Reads unscaled temperature or pressure data from sensor
//! based on previous convserion command sent. 
//!
//! @scope Private
//! 
//! @return uint32_t (Results of previous calculation.  If
//! data requested prior to calculation finishing, sensor ressponds with 0)
//********************************************************
uint32_t MS5x::read_adc()
{
  uint32_t value=0;
  uint32_t r=0;
  
  send_cmd(MS5xxx_CMD_ADC_READ);
  uint8_t ret = _Wire->requestFrom(_I2CADDR, 3); // Request 3 bytes from sensor
  if (ret == 3) {
	r = _Wire->read();
	value = (r<<16);
	r = _Wire->read();
	value += (r<<8);
	r = _Wire->read();
	value += r;
  } else {
	  value = 0;
  }
 
  return value;
}

//******************************************************** 
//! @brief performs calculation of CRC based on factory set calibration coefficents and confirms it is
//! equal to the factory set CRC value.
//! What does this actually do?  It's not like it confirms sensor was calibrated correctly in the factory,
//! just that the CRC was calculated correctly based on whatever values they put into calibration coefficents.
//! If this comes back false, they couldn't even get that level of check right and the sensor definitely shouldn't
//! be trusted.  If it comes back true... it's not like this actually proves they calibrated it correctly.
//!
//! @scope Public
//! 
//! @return bool (True if CRC calculation matches CRC value in sensor's PROM)
//********************************************************
bool MS5x::checkCRC()  {
	bool statusRet = false;
	if ((Calc_CRC4(_COEFFICENTS) == Read_CRC4())) statusRet = true;
	return statusRet;
}

//******************************************************** 
//! @brief Returns true when Temperature and Pressure values have been initialzed
//!
//! @scope Public
//! 
//! @return bool (Temperature and Pressure values Initialized
//********************************************************
bool MS5x::isReady() {
	return _INITALIZED;
}

//******************************************************** 
//! @brief Controls checking the sensor for updates.  When conversions are completed
//! based on oversampling settings, sets updated to true at end of conversion process.  
//! No additional conversions will be calculated until Readout() function is called.
//!
//! @scope Public
//! 
//! @return void
//********************************************************
void MS5x::checkUpdates() {
	uint32_t currMillis = millis();
	if (_READSTEP > 3) _READSTEP = 0;
	if ((uint32_t)(currMillis - _PREVSTEPTIME) >= _READDELAY) {
		switch (_READSTEP)
		{
			case -1: // -1 is set when reset() function is called.  Restores read delay settings
				ReadProm(); // Read Prom doesn't need to be called each cycle
				_READDELAY = _READDELAYPREV;
				_READSTEP += 1;
				_PREVSTEPTIME = currMillis;
				break;
			case 0: // Send command to do Temperature calculations
				send_cmd(MS5xxx_CMD_ADC_CONV+MS5xxx_CMD_ADC_D2+_SAMPLERATIO);
				_D1 = _D2 = 0;
				_READSTEP += 1;
				_PREVSTEPTIME = currMillis;
				break;
			case 1: // Send command to read Temperature then Send command to do Pressure calculations
				_D2 = read_adc();
				send_cmd(MS5xxx_CMD_ADC_CONV+MS5xxx_CMD_ADC_D1+_SAMPLERATIO);
				_READSTEP += 1;
				_PREVSTEPTIME = currMillis;
				break;
			case 2: // Send command to read Temperature then do conversion math
				_D1 = read_adc();
				// If D1 and D2 = 0 then there was an error communicating with the sensor or the conversion process
				// Set Press and temp to zero and set readdelay to some absurd value to reset conversion process
				if (_D1 == 0 || _D2 == 0) { 
					_PRESS = 0;
					_TEMP = 0;
					_READDELAY = 50;
					_PREVSTEPTIME = currMillis;
				} else { // Pressumably everything is fine, continue with conversion process
					Readout();
					_READSTEP += 1;
					_PREVSTEPTIME = currMillis;
				}
				break;
			case 3: // Once polling delay is finished, restart process
				_READDELAY = _READDELAYPREV;
				_READSTEP += 1;
				break;
		}			
	}
}

//******************************************************** 
//! @brief Converts unscaled temperature and pressure readings into scaled values
//! pressure reading is temperature compensated.
//!
//! @scope Private
//! 
//! @return void
//********************************************************
void MS5x::Readout() {
	
	// While sensor document states these variables should be int32 and int64 unless everything is
	// int64 calculation errors occur.  To ensure compatibility across multiple platforms, I've chosen to
	// make their types as doubles.
	double dT = 0; // Difference in actual vs reference temperature
	double OFF = 0; // Offset at actual temperature
	double SENS = 0; //Sensitivy at actual temperature
	
	// When Pressure and Temperature start off as doubles, it tried to calculate it beyond it's precision
	// keep it to int32_t and then multiply by 0.01 to get only the level of precision the sensor has.
	int32_t pressure = 0;
	int32_t temp = 0;

	// Calculate 1st order pressure and temperature based on sensor used.
	dT=(float)_D2-(float)_COEFFICENTS[5]*256.0f; // _D2 - Tref = _D2 - C5 * 2^8
	
	#ifdef MS5611
		OFF=(float)_COEFFICENTS[2]*65536.0f+(dT*(float)_COEFFICENTS[4])/128.0f; // OFFt1 + TCO * dT = C2 * 2^16 + (C4 * dT) / 2^7
		SENS=(float)_COEFFICENTS[1]*32768.0f+(dT*(float)_COEFFICENTS[3])/256.0f; // SENSt1 + TCS * dT = C1 * 2^15 + (C3*dT) / 2^8
	#else
		OFF=(float)_COEFFICENTS[2]*131072.0f+(float)dT*_COEFFICENTS[4]/64.0f; // OFFt1 + TCO * dT = C2 * 2^17 + (C4 * dT) / 2^6
		SENS=(float)_COEFFICENTS[1]*65536.0f+(float)dT*_COEFFICENTS[3]/128.0f; // SENSt1 + TCS * dT = C1 * 2^16 + (C3*dT) / 2^7
	#endif
	
	temp=(2000.0f+(dT*(float)_COEFFICENTS[6])/8388608.0f)+(float)_TOFFSET; // 20C + dT * TEMPSENS = 2000 + dT * C6 / 2^23
	
	 
	// Perform higher order corrections based on sensor used.
	double T2=0., OFF2=0., SENS2=0.;
	if(temp<2000) {
		T2=dT*dT/2147483648.0f; // dT^2 / 2^31
		#ifdef MS5611
			OFF2=5.0f*((temp-2000.0f)*(temp-2000.0f))/2.0f; // 5 * (temp - 2000)^2 / 2
			SENS2=5.0f*((temp-2000.0f)*(temp-2000.0f))/4.0f; // 5 * (temp - 2000)^2 / 4
		#else
			OFF2=61.0f*(temp-2000.0f)*(temp-2000.0f)/16.0f; // 61 * (temp - 2000)^2 / 4
			SENS2=2.0f*(temp-2000.0f)*(temp-2000.0f); // 2 * (temp - 2000)^2
		#endif
	  
	  if(temp<-1500.0f) {
		#ifdef MS5611
			OFF2+=7.0f*((temp+1500.0f)*(temp+1500.0f)); // OFF2 + 7 * (temp + 1500)^2
			SENS2+=11.0f*((temp+1500.0f)*(temp+1500.0f))/2.0f; // SENS2 + 11 * (temp + 1500) ^2 /2
		#else
			OFF2+=15.0f*(temp+1500.0f)*(temp+1500.0f);
			SENS2+=8.0f*(temp+1500.0f)*(temp+1500.0f);
		#endif
	    
	  }
	}
	
	// Apply any required offsets from higher order corrections
	temp-=T2;
	OFF-=OFF2;
	SENS-=SENS2;
	
	// Calculate pressure
	 pressure=((_D1*SENS/2097152.0f-OFF)/32768.0f)+_POFFSET; // (((_D1*SENS)/pow(2,21)-OFF)/pow(2,15))

	// Convert temperature to Celcius
	_TEMP = temp * 0.01f;

	// Apply any required unit conversions for Presure or Temperature
	switch (_PTYPE)
	{
		case 0: _PRESS = pressure; break; // Pascals
		case 1: _PRESS = pressure * 0.01f; break; // Millibars (also Hectioascals)
		case 2: _PRESS = pressure / 3386.3886667; break; // Inches Hg
	}
	
	switch (_TTYPE)
	{
		case 0: break; // Celcius
		case 1: _TEMP = _TEMP * 9.0f/5.0f +32.0f; break; // Fahrenheit
		case 2: _TEMP += 273.15f; break; // Kelvin
	}
	
	_INITALIZED = true; // Temperature and Pressure has been calculated at least once and valid date is provided by GetTemp and GetPress functions
	
	// After conversion process, save the ReadDelay value, then substitute in any desired delay between pollings
	_READDELAYPREV = _READDELAY;
	
	if (_POLLDELAY < _READDELAYPREV) {
		_READDELAY = 0; // ReadDelay should never be less than zero
	} else {
		_READDELAY = _POLLDELAY-(_READDELAYPREV*2); // Subtract the conversion time to have a consistent pollDelay between readings
	}
	_PREVSTEPTIME = millis();
}

//******************************************************** 
//! @brief Returns sensor pressure, must be updated by calling Readout()
//!
//! @scope Public
//! 
//! @return double (Sensor's Pressure)
//********************************************************
double MS5x::GetPres() {
	return _PRESS;
}

//******************************************************** 
//! @brief Returns sensor temperature, must be updated by calling Readout()
//!
//! @scope Public
//! 
//! @return double (Sensor's Temperature)
//********************************************************
double MS5x::GetTemp() {
	return _TEMP;
}

//******************************************************** 
//! @brief Returns altitude based on current pressure, optionally applies temperature correction
//!
//! @scope Public
//! 
//! @return double (Calculate Altitude)
//********************************************************
double MS5x::getAltitude(bool temperatureCorrected) {
	double temperature = 0;
	
	if (_PRESS == 0) {
		return -1; // Pressure hasn't been initailized yet, return -1
	}
	
	// Temperature correction for the altitude can create a lot of wobble to altitude calculations
	// make sure you do some averaging of temperature to prevent random spikes from creating huge
	// swings in altitude.
	if (temperatureCorrected) {
		if (_TEMP == 0) {
			return -2; // Temperature has not been intialized yet return -2
		} else {
			if (_TTYPE != 2) { // Only convert if temperature isn't already in Kelvin
				switch (_TTYPE) { // Convert from existing units to C
					case 0: temperature += 273.15f; break; // C to K
					case 1: temperature = (_TEMP + 459.67) * 5.0f/9.0f; break; // F to K
					case 2: break; // This is here just for safety
				}
			} else {
				temperature = _TEMP;
			}
			return ((pow(_SEALEVELPRESS/(_PRESS), 0.19022256)-1.)*temperature)/0.0065;
		}
	} else {
		return 44330. * (1. - pow(_PRESS / _SEALEVELPRESS, 0.19029495));
	}
}

//******************************************************** 
//! @brief Returns calculated pressure at SeaLevel based on
//! the current pressure reading and provided altitude
//!
//! @scope Public
//! 
//! @return double (Calculated Pressure at SeaLevel)
//********************************************************
double MS5x::getSeaLevel(double altitude)
{	
	// If pressure hasn't been initailized yet, return -1
	if (_PRESS == 0) {
		return -1;
	}
	
	// This formula makes a lot of assumptions, but even if it isn't perfect, by setting a baseline
	// your subsequent altitude change calculations will be accurate, provided the weather doesn't change.
    _SEALEVELPRESS = _PRESS / pow((1.0 - (altitude / 44330.0)), 5.255);
	
	return _SEALEVELPRESS;
}
