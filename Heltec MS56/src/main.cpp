#include <Arduino.h>
#include "MS5611.h"

MS5611 MS5611(0x77);

uint32_t start, stop;

void test()
{
	start = micros();
	int result = MS5611.read();
	stop = micros();
	if (result != MS5611_READ_OK)
	{
		Serial.print("Error in read: ");
		Serial.println(result);
	}
	else
	{
		Serial.print("T:\t");
		Serial.print(MS5611.getTemperature(), 2);
		Serial.print("\tP:\t");
		Serial.print(MS5611.getPressure(), 2);
		Serial.print("\tt:\t");
		Serial.print(stop - start);
		Serial.println();
	}
}

void setup()
{
	Serial.begin(115200);
	Serial.println("Testing MS56");

	if (MS5611.begin() == true)
	{
		Serial.println("MS5611 found.");
	}
	else
	{
		Serial.println("MS5611 not found. halt.");
		while (1);
	}
	MS5611.setOversampling(OSR_ULTRA_HIGH);
}

void loop()
{
	test();
	delay(1000);
}